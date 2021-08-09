bool getLB = false;
bool getHB = false;
char LBbytes[256];
char HBbytes[256];

uint8_t lineReceived = 0;
uint8_t tilePerLineReceived = 0;
uint8_t tileLine[8][160];

int totalHeight = 0;

TaskHandle_t TaskWriteImage;
/*******************************************************************************
  Convert to BMP
*******************************************************************************/
void ConvertFilesBMP()
{

  Serial.println("P2");
  Serial.println("160 144");
  Serial.println("3");
  
  byte ch = 0;
  bool skipLine = false;
  int  lowNibFound = 0;
  uint8_t byt = 0;
  unsigned int bytec = 0;

  File root = FSYS.open("/dumps");
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }
  File filedir = root.openNextFile();
  while (filedir) {
    if (!filedir.isDirectory()) {
      char path[14];
      sprintf(path, "/dumps/%s", filedir.name());

      File file = FSYS.open(path);
      if (!file) {
        Serial.println("Failed to open file for reading");
        return;
      }
      while (file.available())
      {
        ch = ((byte)file.read());
        gbpdecoder_gotByte(ch);
      }
      file.close();
      Serial.println("");
    }
    filedir = root.openNextFile();
  }
  
}

/*******************************************************************************
   Decode the Image
*******************************************************************************/
void gbpdecoder_gotByte(const uint8_t bytgb){
  if (gbp_pkt_processByte(&gbp_pktBuff, bytgb, gbp_pktbuff, &gbp_pktbuffSize, sizeof(gbp_pktbuff))){
    if (gbp_pktBuff.received == GBP_REC_GOT_PACKET){
      pktCounter++;
      if (gbp_pktBuff.command == GBP_COMMAND_PRINT){
        //Save as Image File (print the last tile group)
        for (int i = 0; i < 8 ;i++){
          Serial.println(" ");
          for (int j = 0; j < 160 ;j++){
            Serial.print(tileLine[i][j]);            
            Serial.print(" ");
            //          xTaskCreatePinnedToCore(storeData,            // Task function.
//                                              "storeData",          // name of task.
//                                              10000,                // Stack size of task
//                                              (void*)&image_data,   // parameter of the task
//                                              1,                    // priority of the task
//                                              &TaskWriteDump,       // Task handle to keep track of created task
//                                              0);                   // pin task to core 0
          }
        }
      }
    }else{
      // Support compression payload
      while (gbp_pkt_decompressor(&gbp_pktBuff, gbp_pktbuff, gbp_pktbuffSize, &tileBuff)){
        if (gbp_pkt_tileAccu_tileReadyCheck(&tileBuff)){
          // Got Tile
          for (int i = 0 ; i < GBP_TILE_SIZE_IN_BYTE ; i++){
            const uint8_t data_8bit = tileBuff.tile[i];
            parsebyte(data_8bit); 
            if(i == GBP_TILE_SIZE_IN_BYTE-1){
              //Last char of the tile
              //parsebyte(data_8bit);  
            } else {
              //Print char
              //parsebyte(data_8bit);
            }
          }
        }
      }      
    }
  }
}

void parsebyte(uint8_t abyte){
  //Convert int to binary
  uint8_t bitsCount = sizeof(abyte) * 8;
  char str[ bitsCount + 1 ];
  uint8_t i = 0;
  while ( bitsCount-- )
      str[i++] = bitRead( abyte, bitsCount ) + '0';
  str[i] = '\0';
  
  //Get High and Low Bytes
  if(!getLB && !getHB){
    strcpy(LBbytes, str);
    getLB = true;
  }else{
    if(getLB && !getHB){
      strcpy(HBbytes, str);
      getHB = true;
    }
  }

  //Process Pixels of the line
  if(getLB && getHB){

    if (lineReceived > 7){
      lineReceived = 0;
      tilePerLineReceived++;
    }
    if(tilePerLineReceived > 19){
      totalHeight = totalHeight + 8;
      tilePerLineReceived = 0;
  
      for (int i = 0; i < 8 ;i++){
        Serial.println(" ");
        for (int j = 0; j < 160 ;j++){
          Serial.print(tileLine[i][j]);            
          Serial.print(" ");
//          xTaskCreatePinnedToCore(storeData,            // Task function.
//                                  "storeData",          // name of task.
//                                  10000,                // Stack size of task
//                                  (void*)&image_data,   // parameter of the task
//                                  1,                    // priority of the task
//                                  &TaskWriteDump,       // Task handle to keep track of created task
//                                  0);                   // pin task to core 0
        }
      }
    }
    
    char resultbytes[2];
    for (int i = 0 ; i < 8 ; i++){
      sprintf(resultbytes, "%c%c", HBbytes[i],LBbytes[i]);
      switch (atoi(resultbytes)) {
        case 0:
          tileLine[lineReceived][i+(tilePerLineReceived*8)] = 3;
          break;
        case 1:
          tileLine[lineReceived][i+(tilePerLineReceived*8)] = 2;       
          break;
        case 10:
          tileLine[lineReceived][i+(tilePerLineReceived*8)] = 1;
          break;
        case 11:
          tileLine[lineReceived][i+(tilePerLineReceived*8)] = 0;
          break;
        default:
          break;
      }
    }

    lineReceived++;
    
    getLB = false;
    getHB = false;
    memset(LBbytes, 0x00, sizeof(LBbytes));
    memset(HBbytes, 0x00, sizeof(HBbytes));
  }  
}


/*******************************************************************************
   DEBUG
*******************************************************************************/
void clearDumps() {
  unsigned int dumpcount = 0;
  File dumpDir = FSYS.open("/dumps");    
  File file = dumpDir.openNextFile();

  char filename[12]; 

  while(file) {
    sprintf(filename, "/dumps/%s", file.name());
    dumpcount++;    
    file = dumpDir.openNextFile();
    FSYS.remove(filename);
  } 
  Serial.println(((String)dumpcount) + " images deleted on DUMPS");

  dumpcount = 0;
  dumpDir = FSYS.open("/temp");
  file = dumpDir.openNextFile();
  while(file) {
    sprintf(filename, "/temp/%s", file.name());
    dumpcount++;    
    file = dumpDir.openNextFile();
    FSYS.remove(filename);
  }
  Serial.println(((String)dumpcount) + " images deleted on TEMP");
  
  nextFreeFileIndex();
}
