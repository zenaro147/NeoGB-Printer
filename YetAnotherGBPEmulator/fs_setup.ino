/*******************************************************************************
  Initialize File System and SD Card
*******************************************************************************/
void fs_setup() {
  spiSD.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS); //SCK,MISO,MOSI,SS //HSPI1

  FSYS.begin(SD_CS, spiSD);
  if (!FSYS.begin(true)) {
    isFileMounted = true;
    Serial.println("SD Card Mount Failed");
#ifdef USE_OLED
    oled_msg("ERROR", "Can not mount SD Card");
#endif
    return;
  }

  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
#ifdef USE_OLED
    oled_msg("ERROR", "No SD card attached");
#endif
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  Serial.println(" done");
}

/*******************************************************************************
  Get Next File Name
*******************************************************************************/
unsigned int nextFreeFileIndex() {
  int totFiles = 0;
  File root = FSYS.open("/d");
  File file = root.openNextFile();
  while (file) {
    if (file) {
      totFiles++;
    }
    file = root.openNextFile();
  }
  return totFiles + 1;
}

/*******************************************************************************
  "Printer Full" function
*******************************************************************************/
void full() {
  Serial.println("no more space on printer");
  digitalWrite(LED_STATUS_PIN, HIGH);
#ifdef USE_OLED
  oled_msg("Printer is full!", "Rebooting...");
#endif
  delay(5000);
  ESP.restart();
}
