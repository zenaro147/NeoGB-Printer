uint8_t dtpck = 0;
uint8_t inqypck = 0;

const char nibbleToCharLUT[] = "0123456789ABCDEF";
byte img_tmp[6000] = {}; // 1GBC Picute (5.874)

uint8_t chkMargin = 0x00;

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
          oledStateChange(4); //HEX to TXT
        #endif     
        chkHeader = (int)gbp_serial_io_dataBuff_getByte_Peek(2);
        Serial.print("Get Header: ");
        Serial.print(chkHeader);      
        Serial.print(" - ");
        Serial.println(isWriting);

        switch (chkHeader) {
          case 1:
            chkMargin = 0x00;
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
                callNextFile();              
                inqypck=0;
              }
            }
            ////////////////////////////////////////////// FIX for Tales of Phantasia //////////////////////////////////////////////
            break;
          default:
            break;
        } 
        Serial.print("// ");
        Serial.print(pktTotalCount);
        Serial.print(" : ");
        Serial.println(gbpCommand_toStr(gbp_serial_io_dataBuff_getByte_Peek(2)));
        digitalWrite(LED_STATUS_PIN, HIGH);
      }

      // Print Hex Byte
      data_8bit = gbp_serial_io_dataBuff_getByte();
//      Serial.print((char)nibbleToCharLUT[(data_8bit>>4)&0xF]);
//      Serial.print((char)nibbleToCharLUT[(data_8bit>>0)&0xF]);

      if (!isWriting){
        if (chkHeader == 1 || chkHeader == 2 || chkHeader == 4){
          image_data[img_index] = (byte)data_8bit;
          img_index++;
          Serial.println("Record 1, 2, 4");
          if (chkHeader == 2 && pktByteIndex == 7) { 
            chkMargin = (int)((char)nibbleToCharLUT[(data_8bit>>0)&0xF])-'0';
            Serial.println("Get Margin");
          } 
        }
      }
      
      if ((pktByteIndex > 5) && (pktByteIndex >= (9 + pktDataLength))) {
        digitalWrite(LED_STATUS_PIN, LOW);
        if (chkHeader == 2 && !isWriting) {
          Serial.println("Enter Here to Write");
          isWriting=true;
          if((chkMargin == 0 || ((chkMargin == 3 && dtpck == 1) || (chkMargin == 1 && dtpck == 1) || (chkMargin == 1 && dtpck == 6))) && !setMultiPrint){
            setMultiPrint=true;
            dtpck=0x00;
          }else if(chkMargin > 0 && setMultiPrint){
            setMultiPrint=false;
          }
          memcpy(img_tmp,image_data,6000);
          xTaskCreatePinnedToCore(storeData,        // Task function. 
                                  "storeData",      // name of task. 
                                  10000,            // Stack size of task 
                                  (void*)&img_tmp,  // parameter of the task 
                                  1,                // priority of the task 
                                  &TaskWriteDump,   // Task handle to keep track of created task 
                                  0);               // pin task to core 0 
          memset(image_data, 0x00, sizeof(image_data)); 
                 
          dtpck = 0x00;
          inqypck = 0x00;          
        }
//        Serial.println("");
        pktByteIndex = 0;
        pktTotalCount++;
      } else {
//        Serial.print((char)' ');
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
  Serial.println("Enter Store File");
  unsigned long perf = millis();
  int img_index2=img_index;
  byte *image_data2 = ((byte*)pvParameters);
  img_index = 0x00;

  char fileName[31];
  byte inqypck[10] = {B10001000, B00110011, B00001111, B00000000, B00000000, B00000000, B00001111, B00000000, B10000001, B00000000};  
  
  digitalWrite(LED_STATUS_PIN, LOW);

  if(setMultiPrint || totalMultiImages > 1){
    sprintf(fileName, "/dumps/%05d_%05d.txt", freeFileIndex,totalMultiImages);
  }else{
    sprintf(fileName, "/dumps/%05d.txt", freeFileIndex);
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
    Serial.printf("File /dumps/%05d_%05d.txt written in %lums\n", freeFileIndex,totalMultiImages,perf);
  }else{
    Serial.printf("File /dumps/%05d.txt written in %lums\n", freeFileIndex, perf);
  }

  //Check how much space still have
  uint8_t percUsed = fs_info();
  if (percUsed > 10) {
    if(!setMultiPrint){
      freeFileIndex++;
      dumpCount++;
    }else{    
      totalMultiImages++;
    }
    
    //Reset Variables
    digitalWrite(LED_STATUS_PIN, LOW);
    Serial.println("Printer ready.");
    isWriting = false;
  
    vTaskDelete(NULL); 
  } else {
    full();
  }
}



void callNextFile(){
  setMultiPrint = false;
  totalMultiImages = 1;

  freeFileIndex = nextFreeFileIndex();
  //Reset Variables
  // Turn LED ON
  digitalWrite(LED_STATUS_PIN, LOW);
  Serial.println("Printer ready.");
  img_index = 0x00;
  chkHeader = 99;
  isWriting = false;
  
  memset(image_data, 0x00, sizeof(image_data));
  
}
