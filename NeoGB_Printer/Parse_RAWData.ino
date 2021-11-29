uint8_t dtpck = 0;

const char nibbleToCharLUT[] = "0123456789ABCDEF";
byte img_tmp[6000] = {}; // 1GBC Picute (5.874)

TaskHandle_t TaskWriteDump;
/*******************************************************************************
  Recieve Raw Data from GameBoy
*******************************************************************************/
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
    uint8_t data_8bit = 0;
    
    // Display the data payload encoded in hex
    for (int i = 0 ; i < dataBuffCount ; i++) {
      // Start of a new packet
      if (pktByteIndex == 0) {
        pktDataLength = gbp_serial_io_dataBuff_getByte_Peek(4);
        pktDataLength |= (gbp_serial_io_dataBuff_getByte_Peek(5) << 8) & 0xFF00;
        
        #ifdef USE_OLED
          oledStateChange(4); //HEX to BIN
        #endif     
        chkHeader = (int)gbp_serial_io_dataBuff_getByte_Peek(2);

        switch (chkHeader) {
          case 1:
            isPrinting = true;
            chkMargin = 0x00;
            break;
          case 4:
            ////////////////////////////////////////////// FIX for merge print in McDonald's Monogatari : Honobono Tenchou Ikusei Game and Nakayoshi Cooking Series 5 : Cake o Tsukurou //////////////////////////////////////////////
            if (pktDataLength > 0){
              //Count how many data packages was received before sending the print command
              dtpck++;
            }
            ////////////////////////////////////////////// FIX for merge print in McDonald's Monogatari : Honobono Tenchou Ikusei Game and Nakayoshi Cooking Series 5 : Cake o Tsukurou //////////////////////////////////////////////
            break;
          default:
            break;
        }        
        #ifdef LED_STATUS_PIN 
          LED_led_ON(LED_STATUS_PIN);
        #endif
        #if defined(COMMON_ANODE) || defined(COMMON_CATHODE)
          LED_led_ON(LED_STATUS_GREEN);
        #endif 
      }

      // Print Hex Byte
      data_8bit = gbp_serial_io_dataBuff_getByte();

      if (chkHeader == 1 || chkHeader == 2 || chkHeader == 4){
        image_data[img_index] = (byte)data_8bit;
        img_index++;
        if (chkHeader == 2 && pktByteIndex == 7) { 
          chkMargin = (int)((char)nibbleToCharLUT[(data_8bit>>0)&0xF])-'0';
        }
      }
      
      if ((pktByteIndex > 5) && (pktByteIndex >= (9 + pktDataLength))) {
        #ifdef LED_STATUS_PIN 
          LED_led_OFF(LED_STATUS_PIN);
        #endif
        #if defined(COMMON_ANODE) || defined(COMMON_CATHODE)
          LED_led_OFF(LED_STATUS_GREEN);
        #endif 
        if (chkHeader == 2 && !isWriting) {
          memcpy(img_tmp,image_data,6000);
          memset(image_data, 0x00, sizeof(image_data)); 
          isWriting=true;
          if((chkMargin == 0 || ((chkMargin == 3 && dtpck == 1) || (chkMargin == 1 && dtpck == 1) || (chkMargin == 1 && dtpck == 6))) && !setMultiPrint){
            setMultiPrint=true;
            dtpck=0x00;
          }else if(chkMargin > 0 && setMultiPrint){
            setMultiPrint=false;
          }
          xTaskCreatePinnedToCore(storeData,        // Task function. 
                                  "storeData",      // name of task. 
                                  10000,            // Stack size of task 
                                  (void*)&img_tmp,  // parameter of the task 
                                  1,                // priority of the task 
                                  &TaskWriteDump,   // Task handle to keep track of created task 
                                  0);               // pin task to core 0 
          dtpck = 0x00;         
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

/*******************************************************************************
  Write HEX dump file
*******************************************************************************/
void storeData(void *pvParameters) {
  unsigned long perf = millis();
  int img_index2=img_index;
  byte *image_data2 = ((byte*)pvParameters);
  img_index = 0x00;

  char fileName[31];
  byte inqypck[10] = {B10001000, B00110011, B00001111, B00000000, B00000000, B00000000, B00001111, B00000000, B10000001, B00000000};  
  
  if(setMultiPrint || totalMultiImages > 1){
    sprintf(fileName, "/dumps/%05d_%05d.bin", freeFileIndex,totalMultiImages);
  }else{
    sprintf(fileName, "/dumps/%05d.bin", freeFileIndex);
  }
  
  File file = FSYS.open(fileName, FILE_WRITE);
  if (!file) {
    Serial.println("file creation failed");
  }
  file.write(image_data2, img_index2);
  file.write(inqypck, 10);
  file.close();

  perf = millis() - perf;
  if(setMultiPrint || totalMultiImages > 1){
    Serial.printf("File /dumps/%05d_%05d.bin written in %lums\n", freeFileIndex,totalMultiImages,perf);
  }else{
    Serial.printf("File /dumps/%05d.bin written in %lums\n", freeFileIndex, perf);
  }

  //Check how much space still have
  uint8_t percUsed = fs_info();
  if (percUsed > 10) {
    if(!setMultiPrint){
      isPrinting = false;
      freeFileIndex=update_get_next_ID(1);
      dumpCount = update_get_dumps(1);
    }else{    
      totalMultiImages++;
    }
    //Reset Variables
    Serial.println("Printer ready.");
    isWriting = false;
  
  } else {
    full();
  }
  vTaskDelete(NULL);
}

/*******************************************************************************
  Force to call the next file 
  (fix for Mary-Kate and Ashley Pocket Planner / E.T.: Digital Companion /
  McDonald's Monogatari : Honobono Tenchou Ikusei Game)
*******************************************************************************/
void ResetPrinterVariables(){
  Serial.println("Reseting Vars...");
  //Reset Variables
  setMultiPrint = false;
  totalMultiImages = 1;  
  img_index = 0x00;
  chkHeader = 99;
  isWriting = false;  
  isPrinting = false;
  memset(image_data, 0x00, sizeof(image_data));
  
  Serial.println("Printer Ready");
}
