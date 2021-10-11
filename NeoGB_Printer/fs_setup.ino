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
        Serial.println("- failed to open Dump directory");
        if(FSYS.mkdir("/dumps")){
            Serial.println("Dump Dir created");
        } else {
            Serial.println("mkdir failed");
        }
    }else{
      Serial.println("Dump folder already exist.");
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
  imgCount=0;
  do {
    sprintf(path, "/output/%05d.bmp", i);
    if(!FSYS.exists(path)){
      sprintf(path, "/dumps/%05d.txt", i);
      if (!FSYS.exists(path)) {
        sprintf(path, "/dumps/%05d_%05d.txt", i, 1);
        if(!FSYS.exists(path)){
          return i;
        }else{
          dumpCount++;
        }
      }else{
        dumpCount++;
      }
    }else{
      imgCount++;
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

/*******************************************************************************
  "Printer Full" function
*******************************************************************************/
void full() {
  Serial.println("no more space on printer");
  digitalWrite(LED_STATUS_PIN, HIGH);
#ifdef USE_OLED
  oledStateChange(3); //Printer Full
#endif
}
