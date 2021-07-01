#include <stdint.h> // uint8_t
#include <stddef.h> // size_t

#include "gbp_emulator_config.h"
#include "gameboy_printer_protocol.h"
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
 * Custom Sets
*******************************************************************************/
uint8_t chkHeader=99;
uint8_t cmdPRNT=0x00;
bool isWriting = false;
unsigned int nextFreeFileIndex();
unsigned int freeFileIndex = 0;

const char nibbleToCharLUT[] = "0123456789ABCDEF";
byte image_data[83000] = {}; //moreless 14 photos (82.236)
uint32_t img_index=0x00;

SPIClass spiSD(HSPI);
TaskHandle_t TaskWrite;

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
  Initialize File System and SD Card
*******************************************************************************/
void fs_setup() {
  spiSD.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS); //SCK,MISO,MOSI,SS //HSPI1
  FSYS.begin(SD_CS, spiSD);
  if (!FSYS.begin(true)) {
    Serial.println("SD Card Mount Failed");
    return;
  }
  
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE){
      Serial.println("No SD card attached");
      ESP.restart();
      return;
  }

  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
      Serial.println("MMC");
  } else if(cardType == CARD_SD){
      Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
      Serial.println("SDHC");
  } else {
      Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  Serial.println(" done");
}

/*******************************************************************************
  Reset Variables
*******************************************************************************/
void resetValues() {
  memset(image_data, 0x00, sizeof(image_data));
  img_index = 0x00;     
    
  // Turn LED ON
  digitalWrite(LED_STATUS_PIN, false);
  Serial.println("Printer ready.");
  
  cmdPRNT = 0x00;
  chkHeader = 99;  
  isWriting = false;
}

/*******************************************************************************
  Write Dump Function
*******************************************************************************/
unsigned int nextFreeFileIndex() {
  int totFiles = 0;
  File root = FSYS.open("/d");
  File file = root.openNextFile();
  while(file){
    if(file){
      totFiles++;
    }
    file = root.openNextFile();
  }
  return totFiles + 1;
}

void full() {
  Serial.println("no more space on printer");
  digitalWrite(LED_STATUS_PIN, HIGH);
  ESP.restart();
}

void storeData(void *pvParameters)
{
  byte *image_data2 = ((byte*)pvParameters);
  
  unsigned long perf = millis();
  char fileName[31];
    
  sprintf(fileName, "/d/%05d.txt", freeFileIndex);
  digitalWrite(LED_STATUS_PIN, LOW);

  File file = FSYS.open(fileName, "w");
  if (!file) {
    Serial.println("file creation failed");
  }
  //file.write(, img_index);
  for (int i = 0; i <= img_index; i++) {
    file.print((char)nibbleToCharLUT[(image_data2[i]>>4)&0xF]);
    file.print((char)nibbleToCharLUT[(image_data2[i]>>0)&0xF]);    
  }
  file.close();
  
  perf = millis() - perf;
  Serial.printf("File /d/%05d.txt written in %lums\n", freeFileIndex, perf);

  if (freeFileIndex < MAX_IMAGES) { 
    freeFileIndex++;
    resetValues();
    vTaskDelete(NULL); 
  } else {
    Serial.println("no more space on printer\nrebooting...");
    full();
  }    
}


/*******************************************************************************
  Main Setup and Loop
*******************************************************************************/
void setup(void)
{
  // Config Serial
  // Has to be fast or it will not transfer the image fast enough to the computer
  Serial.begin(115200);

  /* Pins from gameboy link cable */
  pinMode(GBP_SC_PIN, INPUT);
  pinMode(GBP_SO_PIN, INPUT);
  pinMode(GBP_SI_PIN, OUTPUT);

  /* Default link serial out pin state */
  digitalWrite(GBP_SI_PIN, LOW);

  /* LED Indicator */
  pinMode(LED_STATUS_PIN, OUTPUT);
  digitalWrite(LED_STATUS_PIN, LOW);

  /* Setup */
  gpb_serial_io_init(sizeof(gbp_serialIO_raw_buffer), gbp_serialIO_raw_buffer);

  /* Attach ISR */
  #ifdef GBP_FEATURE_USING_RISING_CLOCK_ONLY_ISR
    attachInterrupt( digitalPinToInterrupt(GBP_SC_PIN), serialClock_ISR, RISING);  // attach interrupt handler
  #else
    attachInterrupt( digitalPinToInterrupt(GBP_SC_PIN), serialClock_ISR, CHANGE);  // attach interrupt handler
  #endif

  fs_setup();
  freeFileIndex = nextFreeFileIndex();
}

void loop()
{
  gbp_packet_capture_loop();

  // Trigger Timeout and reset the printer if byte stopped being received.
  static uint32_t last_millis = 0;
  uint32_t curr_millis = millis();
  if (curr_millis > last_millis)
  {
    uint32_t elapsed_ms = curr_millis - last_millis;
    if (gbp_serial_io_timeout_handler(elapsed_ms))
    {
      // Timeout code
      digitalWrite(LED_STATUS_PIN, LOW);
    }
  }
  last_millis = curr_millis;

  // Diagnostics Console
  while (Serial.available() > 0)
  {
    switch (Serial.read())
    {
      case '?':
        Serial.println("d=debug, ?=help");
        break;

      case 'd':
        Serial.print("waterline: ");
        Serial.print(gbp_serial_io_dataBuff_waterline(false));
        Serial.print("B out of ");
        Serial.print(gbp_serial_io_dataBuff_max());
        Serial.println("B");
        break;
    }
  };
} // loop()


/******************************************************************************/
inline void gbp_packet_capture_loop() {
  /* tiles received */
  static uint32_t byteTotal = 0;
  static uint32_t pktTotalCount = 0;
  static uint32_t pktByteIndex = 0;
  static uint16_t pktDataLength = 0;
  const size_t dataBuffCount = gbp_serial_io_dataBuff_getByteCount();
  if (
    ((pktByteIndex != 0) && (dataBuffCount > 0)) ||
    ((pktByteIndex == 0) && (dataBuffCount >= 6))
  ) {
    const char nibbleToCharLUT[] = "0123456789ABCDEF";
    uint8_t data_8bit = 0;
    
    // Display the data payload encoded in hex
    for (int i = 0 ; i < dataBuffCount ; i++) {     
      // Start of a new packet
      if (pktByteIndex == 0) {
        pktDataLength = gbp_serial_io_dataBuff_getByte_Peek(4);
        pktDataLength |= (gbp_serial_io_dataBuff_getByte_Peek(5)<<8)&0xFF00;
        
        switch ((int)gbp_serial_io_dataBuff_getByte_Peek(2)) {
          case 1:
          case 2:
          case 4:
            chkHeader = (int)gbp_serial_io_dataBuff_getByte_Peek(2);
            break;
          default:
            break;
        }
                
        digitalWrite(LED_STATUS_PIN, HIGH);
      }

      // Print Hex Byte
      data_8bit = gbp_serial_io_dataBuff_getByte();

      if (!isWriting){
        if (chkHeader == 1 || chkHeader == 2 || chkHeader == 4){
          image_data[img_index] = (byte)data_8bit;
          img_index++;
          if (chkHeader == 2 && pktByteIndex == 7) { 
            cmdPRNT = (int)((char)nibbleToCharLUT[(data_8bit>>0)&0xF])-'0';
          } 
        }
      }
      
      // Splitting packets for convenience
      if ((pktByteIndex > 5) && (pktByteIndex >= (9 + pktDataLength))) {        
        digitalWrite(LED_STATUS_PIN, LOW);
        if (chkHeader == 2) {
          if (cmdPRNT > 0 && !isWriting) {
            gbp_serial_io_print_set();  
            isWriting=true;
            xTaskCreatePinnedToCore(storeData,            // Task function. 
                                    "storeData",          // name of task. 
                                    10000,                // Stack size of task 
                                    (void*)&image_data,   // parameter of the task 
                                    1,                    // priority of the task 
                                    &TaskWrite,           // Task handle to keep track of created task 
                                    0);                   // pin task to core 0 
          }else{
              cmdPRNT = 0x00;
              chkHeader = 99;
              isWriting = false;
              delay(200);
              gbp_serial_io_print_done();
          }
        }
        pktByteIndex = 0;
        pktTotalCount++;
      } else {
        pktByteIndex++; // Byte hex split counter
        byteTotal++; // Byte total counter
      }
    }
  }
}

//inline void gbp_packet_capture_loop()
//{
//  /* tiles received */
//  static uint32_t byteTotal = 0;
//  static uint32_t pktTotalCount = 0;
//  static uint32_t pktByteIndex = 0;
//  static uint16_t pktDataLength = 0;
//  const size_t dataBuffCount = gbp_serial_io_dataBuff_getByteCount();
//  if (
//      ((pktByteIndex != 0)&&(dataBuffCount>0))||
//      ((pktByteIndex == 0)&&(dataBuffCount>=6))
//      )
//  {
//    const char nibbleToCharLUT[] = "0123456789ABCDEF";
//    uint8_t data_8bit = 0;
//    for (int i = 0 ; i < dataBuffCount ; i++)
//    { // Display the data payload encoded in hex
//      // Start of a new packet
//      if (pktByteIndex == 0)
//      {
//        pktDataLength = gbp_serial_io_dataBuff_getByte_Peek(4);
//        pktDataLength |= (gbp_serial_io_dataBuff_getByte_Peek(5)<<8)&0xFF00;
//#if 0
//        Serial.print("// ");
//        Serial.print(pktTotalCount);
//        Serial.print(" : ");
//        Serial.println(gbpCommand_toStr(gbp_serial_io_dataBuff_getByte_Peek(2)));
//#endif
//        digitalWrite(LED_STATUS_PIN, HIGH);
//      }
//      // Print Hex Byte
//      data_8bit = gbp_serial_io_dataBuff_getByte();
//      Serial.print((char)nibbleToCharLUT[(data_8bit>>4)&0xF]);
//      Serial.print((char)nibbleToCharLUT[(data_8bit>>0)&0xF]);
//      // Splitting packets for convenience
//      if ((pktByteIndex>5)&&(pktByteIndex>=(9+pktDataLength)))
//      {
//        digitalWrite(LED_STATUS_PIN, LOW);
//        Serial.println("");
//        pktByteIndex = 0;
//        pktTotalCount++;
//      }
//      else
//      {
//        Serial.print((char)' ');
//        pktByteIndex++; // Byte hex split counter
//        byteTotal++; // Byte total counter
//      }
//    }
//  }
//}
