uint8_t chkHeader = 99;

uint8_t dtpck = 0;
uint8_t inqypck = 0;

const char nibbleToCharLUT[] = "0123456789ABCDEF";
byte image_data[12000] = {}; 
byte img_tmp[12000] = {};
uint32_t img_index = 0x00;
uint8_t cmdPRNT = 0x00;

TaskHandle_t TaskWriteDump;
/*******************************************************************************
  Reset Variables
*******************************************************************************/
void resetValues() {
  img_index = 0x00;

  // Turn LED ON
  digitalWrite(LED_STATUS_PIN, false);
  Serial.println("Printer ready.");

  chkHeader = 99;
  cmdPRNT = 0x00;  
  
  dtpck = 0x00;
  inqypck = 0x00;
  
  isPrinting = false;
  isWriting = false;
}

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

        chkHeader = (int)gbp_serial_io_dataBuff_getByte_Peek(2);

        switch (chkHeader) {
          case 1:
            cmdPRNT = 0x00;
            break;
          case 2:          
            break;
          case 4:
            ////////////////////////////////////////////// FIX for merge print in McDonald's Monogatari : Honobono Tenchou Ikusei Game and Nakayoshi Cooking Series 5 : Cake o Tsukurou //////////////////////////////////////////////
            if (pktDataLength > 0){
              //Count how many data packages was received before sending the print command
              dtpck++;
            }
            ////////////////////////////////////////////// FIX for merge print in McDonald's Monogatari : Honobono Tenchou Ikusei Game and Nakayoshi Cooking Series 5 : Cake o Tsukurou //////////////////////////////////////////////
            break;
          case 15:
            ////////////////////////////////////////////// FIX for Tales of Phantasia //////////////////////////////////////////////
            if (totalMultiImages > 1 && !isWriting && !setMultiPrint){
              inqypck++;
              if(inqypck > 20){
                //Force to write the saves images  
                xTaskCreatePinnedToCore(gpb_mergeMultiPrint,    // Task function. 
                                        "mergeMultiPrint",      // name of task. 
                                        10000,                  // Stack size of task 
                                        NULL,                   // parameter of the task 
                                        1,                      // priority of the task 
                                        &TaskWriteDump,             // Task handle to keep track of created task 
                                        0);                     // pin task to core 0               
                inqypck=0;
              }
            }
            ////////////////////////////////////////////// FIX for Tales of Phantasia //////////////////////////////////////////////
            break;
          default:
            break;
        }  

#ifdef USE_OLED
        if (!isPrinting) {
          isPrinting = true;
          oled_msg("Receiving Data...");
        }
#endif

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

      if ((pktByteIndex > 5) && (pktByteIndex >= (9 + pktDataLength))) {
        digitalWrite(LED_STATUS_PIN, LOW);
        if (chkHeader == 2 && !isWriting) {
          isWriting=true;
          if((cmdPRNT == 0 || ((cmdPRNT == 3 && dtpck == 1) || (cmdPRNT == 1 && dtpck == 6))) && !setMultiPrint){
            setMultiPrint=true;
            dtpck=0x00;
          }else if(cmdPRNT > 0 && setMultiPrint){
            setMultiPrint=false;
          }
          memcpy(img_tmp,image_data,12000);
          xTaskCreatePinnedToCore(storeData,            // Task function. 
                                  "storeData",          // name of task. 
                                  10000,                // Stack size of task 
                                  (void*)&img_tmp,   // parameter of the task 
                                  1,                    // priority of the task 
                                  &TaskWriteDump,           // Task handle to keep track of created task 
                                  0);                   // pin task to core 0 
          memset(image_data, 0x00, sizeof(image_data));
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
void storeData(void *pvParameters)
{
  unsigned long perf = millis();
  byte *image_data2 = ((byte*)pvParameters);
  int img_index2=img_index;
  char fileName[31];
  
  digitalWrite(LED_STATUS_PIN, LOW);

  #ifdef USE_OLED
    oled_msg("Saving RAW file...");
  #endif

  if(setMultiPrint || totalMultiImages > 1){
    sprintf(fileName, "/temp/%05d_%05d.txt", freeFileIndex,totalMultiImages);
  }else{
    sprintf(fileName, "/dumps/%05d.txt", freeFileIndex);
  }
  
  File file = FSYS.open(fileName, FILE_WRITE);
  if (!file) {
    Serial.println("file creation failed");
  }
  file.write(image_data2, img_index2);
  file.close();

  perf = millis() - perf;
  if(setMultiPrint || totalMultiImages > 1){
    Serial.printf("File /temp/%05d_%05d.txt written in %lums\n", freeFileIndex,totalMultiImages,perf);
  }else{
    Serial.printf("File /dumps/%05d.txt written in %lums\n", freeFileIndex, perf);
  }

  if (freeFileIndex < MAX_IMAGES) {
    if(!setMultiPrint && totalMultiImages <= 1){
      freeFileIndex++; 
    }else if (setMultiPrint){    
      totalMultiImages++;
    }
    resetValues();
    vTaskDelete(NULL); 
  } else {
    Serial.println("no more space on printer\nrebooting...");
    full();
  }
}

/*******************************************************************************
  Merge multiple files into one single file
*******************************************************************************/
void gpb_mergeMultiPrint(void *pvParameters){
  byte inqypck[10] = {B10001000, B00110011, B00001111, B00000000, B00000000, B00000000, B00001111, B00000000, B10000001, B00000000};
  img_index = 0;
  memset(image_data, 0x00, sizeof(image_data));
  Serial.println("Merging Files");

  char path[31];
  for (int i = 1 ; i <= totalMultiImages ; i++){
    sprintf(path, "/temp/%05d_%05d.txt", freeFileIndex,i);
    //Read File
    File file = FSYS.open(path);
    while(file.available()){
      image_data[img_index] = ((byte)file.read());
      img_index++;
    }
    file.close();
    FSYS.remove(path);
    
    //Write File
    sprintf(path, "/dumps/%05d.txt", freeFileIndex);
    if(i == 1){
      file = FSYS.open(path, FILE_WRITE);
    }else{
      file = FSYS.open(path, FILE_APPEND);
    }
    file.write(image_data,img_index);
    file.write(inqypck, 10);
    file.close();
    
    memset(image_data, 0x00, sizeof(image_data));
    img_index = 0;
  } 
  
  setMultiPrint = false;
  totalMultiImages = 1;
  
  Serial.printf("File %s written", path);
  if (freeFileIndex < MAX_IMAGES) {
    freeFileIndex++; 
    resetValues();
  } else {
    Serial.println("no more space on printer\nrebooting...");
    full();
  }
  
  vTaskDelete(NULL); 
}

void gbp_detect_multiprint_loop(){
    if(!setMultiPrint && totalMultiImages > 1 && !isWriting){
    #ifdef USE_OLED
      oled_msg("Long Print detected","Merging Files...");
    #endif
    isWriting = true;
    xTaskCreatePinnedToCore(gpb_mergeMultiPrint,    // Task function. 
                    "mergeMultiPrint",              // name of task. 
                    10000,                          // Stack size of task 
                    NULL,                           // parameter of the task 
                    1,                              // priority of the task 
                    &TaskWriteDump,                     // Task handle to keep track of created task 
                    0);                             // pin task to core 0  
  }
}
