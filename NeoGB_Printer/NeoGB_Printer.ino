 #include <stdint.h> // uint8_t
#include <stddef.h> // size_t

#include "config.h"
#include "gameboy_printer_protocol.h"
#include "gbp_pkt.h"
#include "gbp_serial_io.h"
#include "RGB_LED.h"

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

byte image_data[6000] = {}; //1 GBC Picute (5.874)
uint8_t chkHeader = 99;
uint32_t img_index = 0x00;
bool testmode = false;

bool isWriting = false;
bool isConverting = false;

bool isFileSystemMounted = false;

unsigned int nextFreeFileIndex();
unsigned int freeFileIndex = 0;

unsigned int dumpCount = 0;

bool setMultiPrint = false;
unsigned int totalMultiImages = 1;

TaskHandle_t TaskWriteImage;

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
void ICACHE_RAM_ATTR serialClock_ISR(void)
{
  // Serial Clock (1 = Rising Edge) (0 = Falling Edge); Master Output Slave Input (This device is slave)
#ifdef GBP_FEATURE_USING_RISING_CLOCK_ONLY_ISR
  const bool txBit = gpb_serial_io_OnRising_ISR(digitalRead(GBP_SO_PIN));
#else
  const bool txBit = gpb_serial_io_OnChange_ISR(digitalRead(GBP_SC_PIN), digitalRead(GBP_SO_PIN));
#endif
  digitalWrite(GBP_SI_PIN, txBit ? HIGH : LOW);
}

/*******************************************************************************
  Main Setup and Loop
*******************************************************************************/
void setup(void)
{
  // Config Serial
  // Has to be fast or it will not transfer the image fast enough to the computer
  Serial.begin(115200);  
  //Force CPU Frequency to 160MHz instead the default 240MHz. This fix the Mc Donalds game issue.
  //In case of erros, change to 80
  setCpuFrequencyMhz(160); 
  
  /* LED Indicator */
  pinMode(LED_STATUS_RED, OUTPUT);
  pinMode(LED_STATUS_GREEN, OUTPUT);
  pinMode(LED_STATUS_BLUE, OUTPUT);
  RGB_init();
  RGB_blink_white(3, 50, 50);
  
  /* Pin for pushbutton */ 
  pinMode(BTN_PUSH, INPUT);

  /* Setup File System and OLED*/
  #ifdef USE_OLED
    oled_setup();
  #endif
  
  if(!testmode && (digitalRead(BTN_PUSH) == HIGH)){
    testmode = true;
    #ifdef USE_OLED
      oledStateChange(99); //Test
    #endif
  }else{
    #ifdef USE_OLED
      oledStateChange(0); //Splash Screen
    #endif 
  }

  //Force BMP output if no one was defined and the Upscale Factor
  #if !defined(BMP_OUTPUT) && !defined(PNG_OUTPUT)
      #define BMP_OUTPUT
  #endif
  #ifdef BMP_OUTPUT
      #ifndef BMP_UPSCALE_FACTOR
        #define BMP_UPSCALE_FACTOR
      #endif
  #endif
  #ifdef PNG_OUTPUT
      #ifndef PNG_UPSCALE_FACTOR
        #define PNG_UPSCALE_FACTOR
      #endif
  #endif
  
  delay(3000);

  isFileSystemMounted = fs_setup();
  if(isFileSystemMounted){
    uint8_t percUsed = fs_info();
    if (percUsed <= 10) {
      isFileSystemMounted=false;
      full();
    }
    freeFileIndex = nextFreeFileIndex();

    /* Pins from gameboy link cable */
    pinMode(GBP_SC_PIN, INPUT);
    pinMode(GBP_SO_PIN, INPUT);
    pinMode(GBP_SI_PIN, OUTPUT);
    /* Default link serial out pin state */
    digitalWrite(GBP_SI_PIN, LOW);
  
    /* Setup */
    gpb_serial_io_init(sizeof(gbp_serialIO_raw_buffer), gbp_serialIO_raw_buffer);
  
    /* Attach ISR */
    #ifdef GBP_FEATURE_USING_RISING_CLOCK_ONLY_ISR
      attachInterrupt(digitalPinToInterrupt(GBP_SC_PIN), serialClock_ISR, RISING);  // attach interrupt handler
    #else
      attachInterrupt(digitalPinToInterrupt(GBP_SC_PIN), serialClock_ISR, CHANGE);  // attach interrupt handler
    #endif
  
    gbp_pkt_init(&gbp_pktBuff);
    
    RGB_blink(LED_STATUS_GREEN, 3,100,100);
    #ifdef USE_OLED
      oledStateChange(1); //Printer Idle
      GetNumberFiles();
    #endif
  }else{
    RGB_blink(LED_STATUS_RED, 3,200,200);
    #ifdef USE_OLED
      oledStateChange(2); //SD Init Error
    #endif
  }
}

void loop(){
  if(isFileSystemMounted){
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
        RGB_led_OFF(LED_STATUS_GREEN);
        if(!setMultiPrint && totalMultiImages > 1 && !isWriting){
          callNextFile();
        }
      }
    }
    last_millis = curr_millis;  

    // Feature to detect a short press and a Long Press
    if(!isWriting){
      if (digitalRead(BTN_PUSH) == HIGH) {  
        if (buttonActive == false) {  
          buttonActive = true;
          buttonTimer = millis();  
        }  
        if ((millis() - buttonTimer > longPressTime) && (longPressActive == false)) {  
          longPressActive = true;
          //Long press to convert to BMP
          if (!isConverting && (freeFileIndex-1) > 0 && dumpCount > 0){
            Serial.println("Converting to Image File");
            isConverting = true;            
            #ifdef USE_OLED
              oledStateChange(5); //TXT to BMP
            #endif                        
            RGB_blink(LED_STATUS_BLUE, 3,100,100);
            ConvertFilesBMP();
            RGB_blink(LED_STATUS_BLUE, 3,100,100);   
            #ifdef USE_OLED
              oledStateChange(1); //Printer Idle
              GetNumberFiles();
            #endif     
          }
        }  
      } else {  
        if (buttonActive == true) {
          if (longPressActive == true) {
            longPressActive = false;  
          } else {
            delay(500);
            if((totalMultiImages-1) > 1){
              Serial.println("Get next file ID");
              RGB_blink_magenta(3, 100, 100);
              #ifdef USE_OLED
                oledStateChange(8); //Force Next File
              #endif
              delay(1000);
              callNextFile();
              #ifdef USE_OLED
                oledStateChange(1); //Printer Idle
                GetNumberFiles();
              #endif
            }
          }  
          buttonActive = false;  
        }  
      }
    }
  }
}
