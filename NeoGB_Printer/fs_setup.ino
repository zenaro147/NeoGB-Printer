/*******************************************************************************
  Initialize File System and SD Card
*******************************************************************************/
bool fs_setup() {
//void fs_setup() {
  pinMode(SD_CS, OUTPUT);
  spiSD.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS); //SCK,MISO,MOSI,SS //HSPI1

  FSYS.begin(SD_CS, spiSD);
  if (!FSYS.begin(true)) {
    Serial.println("SD Card Mount Failed");
    return false;
  }else{
    uint8_t cardType = SD.cardType();
    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return false;
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
  
    Serial.printf("SD Card Size: %lluMB\n", FSYS.cardSize() / (1024 * 1024));
    Serial.printf("Total space: %lluMB\n", FSYS.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", FSYS.usedBytes() / (1024 * 1024));
    
    File root = FSYS.open("/dumps");
    if(!root){
        Serial.println("- failed to open Dumps directory");
        if(FSYS.mkdir("/dumps")){
            Serial.println("Dumps Dir created");
        } else {
            Serial.println("mkdir failed");
        }
    }else{
      Serial.println("Dumps folder already exist.");
    }
    
    root = FSYS.open("/temp");
    if(!root){
        Serial.println("- failed to open Temp directory");
        if(FSYS.mkdir("/temp")){
            Serial.println("Temp Dir created");
        } else {
            Serial.println("mkdir failed");
        }
    }else{
      Serial.println("Temp folder already exist.");
    }
  
    root = FSYS.open("/output");
    if(!root){
        Serial.println("- failed to open Output directory");
        if(FSYS.mkdir("/output")){
            Serial.println("Output Dir created");
        } else {
            Serial.println("mkdir failed");
        }
    }else{
      Serial.println("Output folder already exist.");
    }
    
    #ifdef BMP_OUTPUT
      root = FSYS.open("/output/bmp");
      if(!root){
          Serial.println("- failed to open Output BMP directory");
          if(FSYS.mkdir("/output/bmp")){
              Serial.println("Output BMP Dir created");
          } else {
              Serial.println("mkdir failed");
          }
      }else{
        Serial.println("Output BMP folder already exist.");
      }
    #endif 

    #ifdef PNG_OUTPUT
      root = FSYS.open("/output/png");
      if(!root){
          Serial.println("- failed to open Output PNG directory");
          if(FSYS.mkdir("/output/png")){
              Serial.println("Output PNG Dir created");
          } else {
              Serial.println("mkdir failed");
          }
      }else{
        Serial.println("Output PNG folder already exist.");
      }
    #endif

    for (int i = 0; i < 100; i++) {
      if (i % 10 == 0) {
        Serial.print(".");
      }
    }
    Serial.println(" done"); 
    
    return true;
  }
}

/*******************************************************************************
  Get Next File Name
*******************************************************************************/
unsigned int nextFreeFileIndex() {
  char path[31];
  int i = 1;
  dumpCount=0;
  do {
    sprintf(path, "/output/bmp/%05d.bmp", i);
    if(!FSYS.exists(path)){
      sprintf(path, "/dumps/%05d.txt", i);
      if (!FSYS.exists(path)) {
        sprintf(path, "/dumps/%05d_%05d.txt", i, 1);
        if(!FSYS.exists(path)){
          Serial.print("Next File: ");
          Serial.println(i);
          return i;
        }else{
          dumpCount++;
        }
      }else{
        dumpCount++;
      }
    }
    i++;
  } while(true);
}

/*******************************************************************************
  Get the store usage
*******************************************************************************/
int fs_info() {
  uint64_t totalBytes=0;
  uint64_t usedBytes=0;
  totalBytes = FSYS.totalBytes();
  usedBytes = FSYS.usedBytes();   
  
  return (int)(
    (((float)totalBytes - (float)usedBytes) / (float)totalBytes) * 100.0
  );
}

#ifdef USE_OLED
/*******************************************************************************
  Get the number of images and dumps to show
*******************************************************************************/
void GetNumberFiles() {
  char dirname[31];
  char path[31];
  char path2[31];
  int i = 0;
  int totalDumps=0;
  int totalImages=0;
  
  sprintf(dirname, "/dumps");
  File root = FSYS.open(dirname);
  File file = root.openNextFile();
  while(file){
    i++;
    file = root.openNextFile();
  }
  for(int x = 1; x <= i; x++){
    sprintf(path, "/dumps/%05d.txt", x);
    sprintf(path2, "/dumps/%05d_00001.txt", x);
    if(FSYS.exists(path) || FSYS.exists(path2)){
      totalDumps++;
    }
  }  

  i=0;
  sprintf(dirname, "/output/bmp");
  root = FSYS.open(dirname);
  file = root.openNextFile();
  while(file){
    i++;
    file = root.openNextFile();
  }
  sprintf(dirname, "/output/png");
  root = FSYS.open(dirname);
  file = root.openNextFile();
  while(file){
    i++;
    file = root.openNextFile();
  }
  for(int x = 1; x <= i; x++){
    sprintf(path, "/output/bmp/%05d.bmp", x);
    sprintf(path2, "/output/png/%05d.png", x);
    if(FSYS.exists(path) || FSYS.exists(path2)){
      totalImages++;
    }
  }
  
  Serial.printf("RAW Files: %d files\n", totalDumps);
  Serial.printf("BMP/PNG Files: %d files\n", totalImages);
  oled_writeNumImages(totalDumps,totalImages);
  
}
#endif

/*******************************************************************************
  "Printer Full" function
*******************************************************************************/
void full() {
  Serial.println("no more space on printer");
  RGB_led_ON(LED_STATUS_RED);
#ifdef USE_OLED
  oledStateChange(3); //Printer Full
#endif
}
