#include <stdint.h> // uint8_t
#include <stddef.h> // size_t

#include "config.h"

#include "gameboy_printer_protocol.h"
#include "gbp_pkt.h"
#include "gbp_serial_io.h"

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
long longPressTime = 2000;
boolean buttonActive = false;
boolean longPressActive = false;

byte image_data[6000] = {}; // 1GBC Picute (5.874)
uint32_t img_index = 0x00;

bool isWriting = false;
bool isConverting = false;

bool isFileSystemMounted = false;

unsigned int nextFreeFileIndex();
unsigned int freeFileIndex = 0;

unsigned int dumpCount = 0;
unsigned int imgCount = 0;

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

  /* Setup File System and OLED*/
#ifdef USE_OLED
  oled_setup();
  oledStateChange(0); //Splash Screen
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
    Serial.printf("RAW Files: %u files\n", dumpCount);
    Serial.printf("BMP Files: %u files\n", imgCount);

    /* Pins from gameboy link cable */
    pinMode(GBP_SC_PIN, INPUT);
    pinMode(GBP_SO_PIN, INPUT);
    pinMode(GBP_SI_PIN, OUTPUT);
  
    /* Default link serial out pin state */
    digitalWrite(GBP_SI_PIN, LOW);
  
    /* LED Indicator */
    pinMode(LED_STATUS_PIN, OUTPUT);
    digitalWrite(LED_STATUS_PIN, LOW);
     
    /* Pin for pushbutton */ 
    pinMode(BTN_PUSH, INPUT);
  
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
    #endif
  }else{
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
        // Timeout code
        Serial.println("Printer Timeout");
        digitalWrite(LED_STATUS_PIN, LOW);
        if(!setMultiPrint && totalMultiImages > 1 && !isWriting){
          callNextFile();
        }
        #ifdef USE_OLED
          oledStateChange(1); //Printer Idle
        #endif  
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
            Serial.println("Converting to BMP");
            isConverting = true;
            delay(2000);              
            #ifdef USE_OLED
              oledStateChange(5); //Converting to Image
            #endif
            ConvertFilesBMP();
//            xTaskCreatePinnedToCore(ConvertFilesBMP,        // Task function. 
//                                    "ConvertFilesBMP",      // name of task. 
//                                    20000,                  // Stack size of task 
//                                    NULL,                   // parameter of the task 
//                                    1,                      // priority of the task 
//                                    &TaskWriteImage,        // Task handle to keep track of created task 
//                                    0);                     // pin task to core 0         
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
              #ifdef USE_OLED
                oledStateChange(7); //Force Next File
                delay(2000);
              #endif
              callNextFile();
              #ifdef USE_OLED
                oledStateChange(1); //Printer Idle
              #endif
            }
          }  
          buttonActive = false;  
        }  
      }
    }
    
  }
}
