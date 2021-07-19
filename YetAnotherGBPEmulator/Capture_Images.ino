/*******************************************************************************
  Write HEX dump file
*******************************************************************************/
void storeData(void *pvParameters)
{
  byte *image_data2 = ((byte*)pvParameters);

  unsigned long perf = millis();
  char fileName[31];

#ifdef USE_OLED
  oled_msg("Saving RAW file...");
#endif

  sprintf(fileName, "/d/%05d.txt", freeFileIndex);
  digitalWrite(LED_STATUS_PIN, LOW);

  File file = FSYS.open(fileName, "w");
  if (!file) {
    Serial.println("file creation failed");
  }
  file.write(image_data2, img_index);
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

  File root = FSYS.open("/d");
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
    if (filedir.isDirectory()) {
//      Serial.print("  DIR : ");
//      Serial.println(filedir.name());
    } else {
//      Serial.print("  FILE: ");
//      Serial.print(filedir.name());
//      Serial.print("  SIZE: ");
//      Serial.println(filedir.size());

      char path[14];
      sprintf(path, "/d/%s", filedir.name());

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



void clearDumps() {
  unsigned int dumpcount = 0;
  File dumpDir = FSYS.open("/d");    
  File file = dumpDir.openNextFile();

  char filename[12]; 

  while(file) {
    sprintf(filename, "/d/%s", file.name());
    dumpcount++;    
    file = dumpDir.openNextFile();
    FSYS.remove(filename);
  }
  Serial.println(((String)dumpcount) + " images deleted");
  nextFreeFileIndex();
}
