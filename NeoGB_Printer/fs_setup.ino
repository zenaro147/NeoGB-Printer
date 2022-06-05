/*******************************************************************************
  Initialize File System and SD Card
*******************************************************************************/
bool fs_setup() {
  pinMode(SD_CS, OUTPUT);
  spiSD.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS); //SCK,MISO,MOSI,SS //HSPI1

  if (!FSYS.begin(SD_CS, spiSD)) {
    Serial.println("SD Card Mount Failed");
    return false;
  }else{
    uint8_t cardType = FSYS.cardType();
    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return false;
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
    
    root = FSYS.open("/www");
    if(!root){
        Serial.println("- failed to open WebServer directory");
        if(FSYS.mkdir("/www")){
            Serial.println("WebServer Dir created");
        } else {
            Serial.println("mkdir failed");
        }
    }else{
      Serial.println("WebServer folder already exist.");
    }
    
    root = FSYS.open("/www/thumb");
    if(!root){
        Serial.println("- failed to open Thumbnail directory");
        if(FSYS.mkdir("/www/thumb")){
            Serial.println("Thumbnail Dir created");
        } else {
            Serial.println("mkdir failed");
        }
    }else{
      Serial.println("Thumbnail folder already exist.");
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
  Get the number of images and dumps to show
*******************************************************************************/
void GetNumberFiles() {
  int totalDumps=get_dumps();
  #ifdef USE_OLED
    oled_writeNumImages(totalDumps);
  #endif
}

/*******************************************************************************
  "Printer Full" function
*******************************************************************************/
void full() {
  Serial.println("no more space on printer");
  #ifdef LED_STATUS_PIN 
    LED_led_ON(LED_STATUS_PIN);
  #endif
  #if defined(COMMON_ANODE) || defined(COMMON_CATHODE)
    LED_led_ON(LED_STATUS_RED);
  #endif
  #ifdef USE_OLED
    oledStateChange(3); //Printer Full
  #endif
}

/*******************************************************************************
  Simple controller to change the Boot Mode
*******************************************************************************/
bool fs_alternateBootMode() {
  char path[]="/bootmode.txt";
  
  if(FSYS.remove(path)){
    return false;
  } else {
    File file = FSYS.open(path,FILE_WRITE);
    file.print("BOOT");
    file.close();
    return true;
  } 
}
