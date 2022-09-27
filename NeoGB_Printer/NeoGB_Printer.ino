 #include <stdint.h> // uint8_t
#include <stddef.h> // size_t

#include "config.h"
#include "./includes/led/LED_status.h"
#include "./includes/StoreIDlib.h"
#include "./includes/gblink/gameboy_printer_protocol.h"
#include "./includes/gblink/gbp_pkt.h"
#include "./includes/gblink/gbp_serial_io.h"

//SD libraries 
#include "SPI.h"
#include "FS.h"
#include "SD.h"
#define FSYS SD

#include <WiFi.h>
#include <ESPmDNS.h> 
#include <WebServer.h>
#include <uri/UriBraces.h>
#include <ArduinoJson.h>

//RTC-NTP Libraries
#include <NTPClient.h>
#include <WiFiUdp.h>

#define numVersion "Ver. 1.6.11"

/*******************************************************************************
 * Invert the SO and SI pins if necessary 
*******************************************************************************/
#ifdef INVERT_SERIAL_PINS
  #define GBP_SO_PIN ESP_MOSI_PIN
  #define GBP_SI_PIN ESP_MISO_PIN
#else
  #define GBP_SO_PIN ESP_MISO_PIN 
  #define GBP_SI_PIN ESP_MOSI_PIN
#endif
#define GBP_SC_PIN ESP_CLK_PIN

/*******************************************************************************
*******************************************************************************/
// Dev Note: Gamboy camera sends data payload of 640 bytes usually
#define GBP_BUFFER_SIZE 650

/* Serial IO */
// This circular buffer contains a stream of raw packets from the gameboy
uint8_t gbp_serialIO_raw_buffer[GBP_BUFFER_SIZE] = {0};
inline void gbp_packet_capture_loop();

/*******************************************************************************
   Process Variables
*******************************************************************************/
gbp_pkt_t gbp_pktBuff = {GBP_REC_NONE, 0};
uint8_t gbp_pktbuff[GBP_PKT_PAYLOAD_BUFF_SIZE_IN_BYTE] = {0};

/*******************************************************************************
   Custom Variables
*******************************************************************************/
// Button Variables
long buttonTimer = 0;
long longPressTime = 2000; //2 seconds
boolean buttonActive = false;
boolean longPressActive = false;

//Data Controller
byte image_data[6000] = {}; //1 GBC Picute (5.874)
uint8_t chkHeader = 99;
uint8_t chkMargin = 0x00;
uint32_t img_index = 0x00;
unsigned int freeFileIndex = 0;
unsigned int dumpCount = 0;
unsigned int totalMultiImages = 1;
bool testmode = false;

//States
bool bootAsPrinter;
bool isPrinting = false;
bool isWriting = false;
bool isConverting = false;
bool isFileSystemMounted = false;
bool setMultiPrint = false;

//Images Output settings
uint8_t scaleBMP = BMP_UPSCALE_FACTOR;
uint8_t scalePNG = PNG_UPSCALE_FACTOR;

//RTC-NTP Variables
WiFiUDP udp;
NTPClient ntp(udp, "0.pool.ntp.org", (RTC_TIMEZONE * 3600)+RTC_TIMEDIFF, 60000); //Create the NTP object
struct tm data; //Create struct to store the Date/Time information
int datetime;
char formatted_datetime[64];

//MISC
//TaskHandle_t TaskWriteImage;
SPIClass spiSD(HSPI);

/*******************************************************************************
  Utility Functions
*******************************************************************************/
const char *gbpCommand_toStr(int val)
{
  switch (val)
  {
    case GBP_COMMAND_INIT    : return "INIT";
    case GBP_COMMAND_PRINT   : return "PRNT";
    case GBP_COMMAND_DATA    : return "DATA";
    case GBP_COMMAND_BREAK   : return "BREK";
    case GBP_COMMAND_INQUIRY : return "INQY";
    default: return "?";
  }
}

/*******************************************************************************
  Interrupt Service Routine
*******************************************************************************/
void IRAM_ATTR serialClock_ISR(void)
{
  // Serial Clock (1 = Rising Edge) (0 = Falling Edge); Master Output Slave Input (This device is slave)
#ifdef GBP_FEATURE_USING_RISING_CLOCK_ONLY_ISR
  const bool txBit = gpb_serial_io_OnRising_ISR(digitalRead(GBP_SI_PIN));
#else
  const bool txBit = gpb_serial_io_OnChange_ISR(digitalRead(GBP_SC_PIN), digitalRead(GBP_SI_PIN));
#endif
  digitalWrite(GBP_SO_PIN, txBit ? HIGH : LOW);
}

/*******************************************************************************
  Main Setup and Loop
*******************************************************************************/
void setup(void){
  Serial.begin(115200); // Config Serial

  LED_init();
  
  /* Pin for pushbutton */ 
  pinMode(BTN_PUSH, INPUT);
  
  /* Setup OLED Display*/
  #ifdef USE_OLED
    oled_setup();
  #endif

  //Check Test Mode
  #ifdef BTN_INVERT
  if(!testmode && (digitalRead(BTN_PUSH) == LOW)){
  #else
  if(!testmode && (digitalRead(BTN_PUSH) == HIGH)){
  #endif
    testmode = true;
    #ifdef USE_OLED
      oledStateChange(99); //Test
    #endif
  }else{
    #ifdef USE_OLED
      oledStateChange(0); //Splash Screen
    #endif 
  }
  
  //Blink the LED to test it. For RGB LED, must in this order: WHITE(all colors), RED, GREEN, BLUE
  #ifdef LED_STATUS_PIN 
    LED_blink(LED_STATUS_PIN, 3, 50, 50);
  #endif
  #if defined(COMMON_ANODE) || defined(COMMON_CATHODE)
    LED_blink(LED_STATUS_RED, LED_STATUS_BLUE, LED_STATUS_GREEN, 3, 50, 50);
    LED_blink(LED_STATUS_RED, 1, 300, 50);
    LED_blink(LED_STATUS_GREEN, 1, 300, 50);
    LED_blink(LED_STATUS_BLUE, 1, 300, 50);
  #endif
  
  delay(500); //Little delay for stetic 

  //Initialize FileSystem
  isFileSystemMounted = fs_setup();
  
  if(isFileSystemMounted){
    ID_file_checker(); //Create/check controller file
    setupImages(); //Get the Image Scale Factors from config file
    
    //Check the bootMode (Printer mode or WiFi mode)
    #ifdef ENABLE_WEBSERVER
      bootAsPrinter = fs_alternateBootMode();
    #else
      bootAsPrinter = true;
    #endif

    //Get the % of use and the next image ID. If full boot as Server
    uint8_t percUsed = fs_info();
    //If full, stucks with a message
    if (percUsed <= 10) {
      bootAsPrinter=false;
      full();
      delay(5000);
    }

    if (bootAsPrinter){
      //initWifi(); //Initiate WiFi and NTP
      WiFi.disconnect();
      Serial.println("-----------------------");
      Serial.println("Booting in printer mode");
      Serial.println("-----------------------");
      
      freeFileIndex = get_next_ID();
      dumpCount = get_dumps();
  
      /* Pins from gameboy link cable */
      pinMode(GBP_SC_PIN, INPUT);
      pinMode(GBP_SI_PIN, INPUT);
      pinMode(GBP_SO_PIN, OUTPUT);
      /* Default link serial out pin state */
      digitalWrite(GBP_SO_PIN, LOW);
    
      /* Setup */
      gpb_serial_io_init(sizeof(gbp_serialIO_raw_buffer), gbp_serialIO_raw_buffer);
    
      /* Attach ISR */
      #ifdef GBP_FEATURE_USING_RISING_CLOCK_ONLY_ISR
        attachInterrupt(digitalPinToInterrupt(GBP_SC_PIN), serialClock_ISR, RISING);  // attach interrupt handler
      #else
        attachInterrupt(digitalPinToInterrupt(GBP_SC_PIN), serialClock_ISR, CHANGE);  // attach interrupt handler
      #endif
    
      gbp_pkt_init(&gbp_pktBuff);
  
      #ifdef USE_OLED
        oledStateChange(1); //Printer Idle
        GetNumberFiles();
      #endif
      setCpuFrequencyMhz(80); //Force CPU Frequency to 80MHz instead the default 240MHz. This fix protocol issue with some games.
      #ifdef ENABLE_RTC
        oledStateChange(12); //Seeking for date/time
        initWifi(); //Initiate WiFi and NTP
        oledStateChange(1); //Printer Idle
        GetNumberFiles();
      #endif
    }
    #ifdef ENABLE_WEBSERVER
      else{
        initWifi(); //Initiate WiFi and NTP
        Serial.println("-----------------------");
        Serial.println("Booting in server mode");
        Serial.println("-----------------------");
        mdns_setup();
        webserver_setup();
        #ifdef USE_OLED
          oledStateChange(9); //Printer Idle as Server
          oled_ShowIP();
        #endif
      }
    #endif
    
    #ifdef LED_STATUS_PIN 
      LED_blink(LED_STATUS_PIN, 3, 100, 100);
    #endif
    #if defined(COMMON_ANODE) || defined(COMMON_CATHODE)
      LED_blink(LED_STATUS_GREEN, 3,100,100);
    #endif
   
  }else{
    #ifdef USE_OLED
      oledStateChange(2); //SD Init Error
    #endif
    #ifdef LED_STATUS_PIN 
      LED_blink(LED_STATUS_PIN, 3, 200, 200);
    #endif
    #if defined(COMMON_ANODE) || defined(COMMON_CATHODE)
      LED_blink(LED_STATUS_RED, 3,200,200);
    #endif
  }
}

void loop(){
  if(isFileSystemMounted){
    if (bootAsPrinter){
      gbp_packet_capture_loop();
      
      // Trigger Timeout and reset the printer if byte stopped being received.
      static uint32_t last_millis = 0;
      uint32_t curr_millis = millis();
      if (curr_millis > last_millis){
        uint32_t elapsed_ms = curr_millis - last_millis;
        if (gbp_serial_io_timeout_handler(elapsed_ms)) {
          //Printer Timeout
          Serial.println("Printer Timeout");
          
          //Reset Values
          chkHeader=99;
          memset(image_data, 0x00, sizeof(image_data)); 
          
          #ifdef USE_OLED
            oledStateChange(7); //Printer Idle (without show number of files - for safity reasons)
          #endif
          #ifdef LED_STATUS_PIN 
            LED_led_OFF(LED_STATUS_PIN);
          #endif
          #if defined(COMMON_ANODE) || defined(COMMON_CATHODE)
            LED_led_OFF(LED_STATUS_GREEN);
          #endif
          if(!setMultiPrint && totalMultiImages > 1 && !isWriting && !isPrinting){
            ResetPrinterVariables();
          }
        }
      }
      last_millis = curr_millis;

      // Feature to detect a short press and a Long Press
      if(!isWriting){
        #ifdef BTN_INVERT
        if(digitalRead(BTN_PUSH) == LOW){
        #else
        if(digitalRead(BTN_PUSH) == HIGH){
        #endif
          if (buttonActive == false) {
            buttonActive = true;
            buttonTimer = millis();  
          }
          if ((millis() - buttonTimer > longPressTime) && (longPressActive == false)){
            longPressActive = true;
            //Long press to convert to Image Files
            if (!isConverting && !isPrinting && (freeFileIndex-1) > 0 && dumpCount > 0){
              Serial.println("Converting to Image File");
              isConverting = true;
              #ifdef USE_OLED
                oledStateChange(5); //BIN to BMP
              #endif
              
              //Remove the interrupt to prevent receive data from Gameboy
              detachInterrupt(digitalPinToInterrupt(GBP_SC_PIN));
              
              setCpuFrequencyMhz(240); //File conversion at full speed !
              ConvertFilesBMP();
              setCpuFrequencyMhz(80); //Force CPU Frequency again to 80MHz instead the default 240MHz. 

              #ifdef GBP_FEATURE_USING_RISING_CLOCK_ONLY_ISR
                attachInterrupt(digitalPinToInterrupt(GBP_SC_PIN), serialClock_ISR, RISING);  // attach interrupt handler
              #else
                attachInterrupt(digitalPinToInterrupt(GBP_SC_PIN), serialClock_ISR, CHANGE);  // attach interrupt handler
              #endif  
              
              #ifdef USE_OLED
                oledStateChange(1); //Printer Idle
                GetNumberFiles();
              #endif     
            }
          }  
        }else{
          if (buttonActive == true) {
            if (longPressActive == true) {
              longPressActive = false;  
            } else {
              delay(500);
              if((totalMultiImages-1) > 1 && chkMargin == 0 && isPrinting){
                Serial.println("Getting next file ID");
                
                #ifdef LED_STATUS_PIN 
                  LED_led_ON(LED_STATUS_PIN);
                #endif
                #if defined(COMMON_ANODE) || defined(COMMON_CATHODE)
                  LED_led_ON(LED_STATUS_RED, LED_STATUS_BLUE);
                #endif                      
                #ifdef USE_OLED
                  oledStateChange(8); //Force Next File
                #endif
                delay(1000);
                dumpCount = update_get_dumps(1);
                freeFileIndex=update_get_next_ID(1);
                ResetPrinterVariables();
                
                #ifdef USE_OLED
                  oledStateChange(1); //Printer Idle
                  GetNumberFiles();
                #endif
                
                #ifdef LED_STATUS_PIN 
                  LED_led_OFF(LED_STATUS_PIN);
                #endif
                #if defined(COMMON_ANODE) || defined(COMMON_CATHODE)
                  LED_led_OFF(LED_STATUS_RED, LED_STATUS_BLUE);
                #endif  
                
              }
            }  
            buttonActive = false;  
          }  
        }
      }
    }else{
      //WebServer Stuffs Here
      #ifdef ENABLE_WEBSERVER
        webserver_loop();
      #endif
    }   
  }
}
