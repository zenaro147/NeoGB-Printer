#include "./includes/gblink/gbp_tiles.h"
#include "./includes/image/gbp_bmp.h"
#include "./includes/image/Upscalerlib.h"

uint8_t pktCounter = 0; // Dev Varible

uint32_t palletColor[4] = {0xFFFFFF,0xAAAAAA,0x555555,0x000000};
uint8_t gbp_pktbuffSize = 0;
gbp_pkt_tileAcc_t tileBuff = {0};
gbp_tile_t gbp_tiles = {0};
gbp_bmp_t  gbp_bmp = {0};

static void gbpdecoder_gotByte(const uint8_t bytgb);

File fileBMP;
char fileBMPPath[30];

uint8_t numfiles = 0;
uint8_t actualfile = 0;

/*******************************************************************************
  Convert to BMP
*******************************************************************************/
void ConvertFilesBMP(){
  #ifdef LED_STATUS_PIN
    LED_blink(LED_STATUS_PIN, 3,100,100);
  #endif
  #if defined(COMMON_ANODE) || defined(COMMON_CATHODE)
    LED_blink(LED_STATUS_BLUE, 3,100,100);
  #endif
  
  unsigned long perf;

  //Clear variables
  memset(image_data, 0x00, sizeof(image_data));
  img_index = 0;

  //Create some vars to use in the process
  char path[30];
  char pathOutput[30];  

  //Get the first dump ID
  char pathcheck1[30];
  char pathcheck2[30];
  int firstDumpID = 1;

  if(testmode){
    sprintf(fileBMPPath, "/temp/%05d.bmp", 0);
    if(!FSYS.exists(fileBMPPath)){
      byte image_test[] = {0x88, 0x33, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x88, 0x33, 0x04, 0x00, 0x80, 0x02, 0xC3, 0xD8, 0xC3, 0xD8, 0xC3, 0xD8, 0xC3, 0xD8, 0xC3, 0xD8, 0xC3, 0xD8, 0xC3, 0xD8, 0xC3, 0xD8, 0x18, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x18, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x18, 0x00, 0x3C, 0xC0, 0xC0, 0x06, 0x00, 0x06, 0x00, 0xC0, 0x00, 0xE0, 0x00, 0xE0, 0x00, 0xC0, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x01, 0x01, 0x03, 0x02, 0x07, 0x04, 0x0E, 0x0F, 0x1F, 0x31, 0x3E, 0x60, 0x7D, 0xC0, 0xF9, 0x84, 0xF3, 0x0F, 0xEF, 0x1F, 0xF8, 0x34, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0xF0, 0xF0, 0xF8, 0xF8, 0xF8, 0xF8, 0x80, 0x80, 0x00, 0x05, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x00, 0x55, 0x00, 0x00, 0xFF, 0xFF, 0xFE, 0x9C, 0xBE, 0xDC, 0xFE, 0x9C, 0xBE, 0xDC, 0xFE, 0x80, 0x00, 0x55, 0x00, 0x00, 0xC0, 0xC0, 0xC0, 0xC0, 0xCF, 0xCF, 0xDF, 0xD8, 0xDF, 0xD3, 0xD7, 0xDB, 0x00, 0x55, 0x00, 0x00, 0x07, 0x07, 0x07, 0x04, 0xE7, 0xE6, 0xF7, 0x76, 0xB3, 0x32, 0xB3, 0x32, 0x00, 0x55, 0x00, 0x00, 0xE7, 0xE7, 0x67, 0x64, 0x67, 0x66, 0x67, 0x66, 0x63, 0x62, 0x63, 0x62, 0x00, 0x55, 0x00, 0x00, 0xE0, 0xE0, 0x60, 0x60, 0x63, 0x63, 0x67, 0x66, 0x67, 0x64, 0x65, 0x66, 0x00, 0x55, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0xF9, 0xF9, 0xFD, 0x1D, 0xED, 0xCD, 0xED, 0xCD, 0x00, 0x55, 0x00, 0x00, 0xF0, 0xF0, 0xB0, 0x30, 0xB0, 0x30, 0xB0, 0x30, 0xB0, 0x30, 0xB0, 0x30, 0x00, 0x00, 0x01, 0x41, 0x02, 0x02, 0x02, 0x42, 0x02, 0x02, 0x02, 0x42, 0x02, 0x02, 0x02, 0x42, 0xFE, 0xFE, 0x83, 0xFF, 0xFE, 0x82, 0x82, 0xFE, 0xFE, 0xFE, 0x82, 0xFE, 0xFE, 0xFE, 0x82, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0xE2, 0x80, 0xE0, 0x80, 0xE0, 0x80, 0xE0, 0x80, 0x03, 0x00, 0x00, 0x60, 0x00, 0x60, 0x00, 0x03, 0x00, 0x07, 0x00, 0x07, 0x00, 0x03, 0x60, 0x60, 0x00, 0x18, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x18, 0x00, 0x80, 0x00, 0x80, 0x18, 0x18, 0x3C, 0x3C, 0xC3, 0x1B, 0xC3, 0x1B, 0xC3, 0x1B, 0xC3, 0x1B, 0xC3, 0x1B, 0xC3, 0x1B, 0xC3, 0x1B, 0xC3, 0x1B, 0xC3, 0xD8, 0xC3, 0xD8, 0xC3, 0xD8, 0xC3, 0xD8, 0xC3, 0xD8, 0xC3, 0xD8, 0xC3, 0xD8, 0xC3, 0xD8, 0x00, 0x3C, 0x00, 0x18, 0x00, 0x01, 0x00, 0x01, 0x18, 0x18, 0x3C, 0x3C, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x06, 0x00, 0xC0, 0x00, 0xE0, 0x00, 0xE0, 0x00, 0xC0, 0x06, 0x06, 0x06, 0x06, 0xC0, 0xC0, 0x07, 0x04, 0x05, 0x07, 0x07, 0x07, 0x43, 0x03, 0x01, 0x03, 0x01, 0x01, 0x03, 0x03, 0x00, 0x00, 0xFD, 0xE1, 0xFD, 0xF1, 0xCF, 0xF0, 0xFD, 0xCE, 0xFF, 0xAF, 0xBF, 0xC3, 0x7F, 0xE0, 0x1F, 0x3F, 0xE0, 0xE0, 0x70, 0x98, 0xF8, 0x08, 0xF0, 0x18, 0xE0, 0xF0, 0x80, 0xC5, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x15, 0xBE, 0xDC, 0xFE, 0x9C, 0xBE, 0xDC, 0xFE, 0x9C, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x55, 0xDF, 0xD0, 0xD7, 0xDB, 0xDF, 0xD3, 0xDF, 0xD8, 0xDF, 0xDF, 0xCF, 0xCF, 0x00, 0x00, 0x00, 0x55, 0xB3, 0x32, 0xF3, 0xF2, 0x33, 0x32, 0x73, 0x72, 0xF3, 0xF3, 0xE3, 0xE3, 0x00, 0x00, 0x00, 0x55, 0x63, 0x62, 0x63, 0x62, 0x63, 0x62, 0x63, 0x62, 0xE3, 0xE3, 0xE3, 0xE3, 0x00, 0x00, 0x00, 0x55, 0x67, 0x64, 0x65, 0x66, 0x67, 0x64, 0x67, 0x66, 0xE7, 0xE7, 0xE3, 0xE3, 0x00, 0x00, 0x00, 0x55, 0xED, 0xCD, 0xED, 0xCD, 0xED, 0xCD, 0xFD, 0x1D, 0xFD, 0xFD, 0xF9, 0xF9, 0x00, 0x00, 0x00, 0x55, 0xB0, 0x30, 0xB0, 0x30, 0xF0, 0xF0, 0xB0, 0x30, 0xF0, 0xF0, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x55, 0x02, 0x02, 0x02, 0x42, 0x02, 0x02, 0x02, 0x42, 0x02, 0x02, 0x02, 0x42, 0x03, 0x03, 0x00, 0x40, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x06, 0xC0, 0x06, 0xC0, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xE0, 0x80, 0xE0, 0x80, 0xE0, 0x80, 0xE2, 0x80, 0xE0, 0x80, 0xE0, 0x80, 0xE0, 0x80, 0xE0, 0x00, 0x60, 0x60, 0x03, 0x03, 0x07, 0x07, 0x07, 0x07, 0x03, 0x03, 0x60, 0x00, 0x60, 0x00, 0x03, 0x00, 0x3C, 0x3C, 0x18, 0x18, 0x80, 0x80, 0x80, 0x80, 0x18, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x18, 0x00, 0xC3, 0x1B, 0xC3, 0x1B, 0xC3, 0x1B, 0xC3, 0x1B, 0xC3, 0x1B, 0xC3, 0x1B, 0xC3, 0x1B, 0xC3, 0x1B, 0x00, 0x00, 0x00, 0x00, 0x88, 0x33, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x88, 0x33, 0x02, 0x00, 0x04, 0x00, 0x01, 0x00, 0xE4, 0x7F, 0x6B, 0x01, 0x00, 0x00};
      for(int bytePos=0; bytePos < sizeof(image_test); bytePos++){
        gbpdecoder_gotByte(image_test[bytePos]);       
      }
    }
    if(scaleBMP > 0){
      //Create a 4bits BMP and resize the image
      sprintf(pathOutput, "/output/bmp/%05d.bmp", 0);
      bmp_upscaler(fileBMPPath,pathOutput,scaleBMP);
    }
    if(scalePNG > 0){
      sprintf(pathOutput, "/output/png/%05d.png", 0);
      png_upscaler(fileBMPPath,pathOutput,scalePNG);
    }
    FSYS.remove(fileBMPPath);             
    testmode = false;
  }

  //Get the first image ID to process
//  do {
//    sprintf(pathcheck1, "/dumps/%05d.bin", firstDumpID);
//    sprintf(pathcheck2, "/dumps/%05d_00001.bin", firstDumpID);
//    if (FSYS.exists(pathcheck1) || FSYS.exists(pathcheck2)) {
//      break;
//    }
//    firstDumpID++;
//  } while(true);
  firstDumpID=get_next_ID()-get_dumps(); 
  
  //Loop to check the availables files in Dump Folder based on nextFreeFileIndex function
  for(int i = firstDumpID; i < freeFileIndex; i++){
    #ifdef USE_OLED
      oledStateChange(5); //BIN to BMP
    #endif

    //Reset the counter for the number of files
    numfiles=0;
    actualfile=0;

    //Check if the file is a long print or a single file
    sprintf(path, "/dumps/%05d.bin", i);
    if(FSYS.exists(path)) {
      numfiles=1;
    }else{     
      //If is long print, count how many files exists
      sprintf(path, "/dumps/%05d_%05d.bin", i, 1);
      if(FSYS.exists(path)){
        for(int filecount=1; filecount <= 100; filecount++){
          sprintf(path, "/dumps/%05d_%05d.bin", i, filecount);
          if(!FSYS.exists(path)){
            break; //Exit from the loop to check the ammount of files in a multiprint
          }else{
            numfiles++;
          }
        }
      }else{
        break; //Exit from the loop, because don't have more files
      }
    }
    
    // Loop to parse the files based on the previous results
    for (int z = 1; z <= numfiles; z++){
      perf = millis();
      
      actualfile = z;
      //Check if is a single file or a long print
      if (numfiles == 1){
        sprintf(path, "/dumps/%05d.bin", i);
      }else{
        sprintf(path, "/dumps/%05d_%05d.bin", i, z);
      }
      sprintf(fileBMPPath, "/temp/%05d.bmp", i);
      
      Serial.printf("Parsing File: %s to %s",path,fileBMPPath);
      
      //Open the file and copy it to the memory
      File file = FSYS.open(path);
      if(!file){
          Serial.println("Failed to open file for reading");
      }
      while(file.available()){
        image_data[img_index] = ((byte)file.read());
        img_index++;
      }
      file.close();
           
      //Send each byte to parse the tile
      for(int bytePos=0; bytePos < img_index; bytePos++){
        gbpdecoder_gotByte(image_data[bytePos]);       
      }

      //clear the variables
      memset(image_data, 0x00, sizeof(image_data));
      img_index = 0;
      
      perf = millis() - perf;
      Serial.printf("... Done! in %lums\n",perf);
     
    
      #ifdef LED_STATUS_PIN 
        LED_blink(LED_STATUS_PIN,1,100,50);
      #endif
      #if defined(COMMON_ANODE) || defined(COMMON_CATHODE)
        LED_blink(LED_STATUS_BLUE,1,100,50);
      #endif
    }
    
    delay(100);

    //Generate the thumbnail for the Webserver
    sprintf(pathOutput, "/www/thumb/%05d.png", i);
    png_upscaler(fileBMPPath,pathOutput,1);
    
    //Create a 4bits BMP and resize the image
    if(scaleBMP > 0){
      #ifdef USE_OLED
        oledStateChange(10); //24bits-BMP to 4bits-BMP
      #endif      
      sprintf(pathOutput, "/output/bmp/%05d.bmp", i);
      Serial.printf("Saving BMP-4bits image in: %s",pathOutput);
      perf = millis();
      bmp_upscaler(fileBMPPath,pathOutput,scaleBMP);
      perf = millis() - perf;
      Serial.printf("... Done! in %lums\n",perf);
      
      #ifdef LED_STATUS_PIN 
        LED_blink(LED_STATUS_PIN,1,100,50);
      #endif
      #if defined(COMMON_ANODE) || defined(COMMON_CATHODE)
        LED_blink(LED_STATUS_BLUE,1,100,50);
      #endif
    }
    //Create a PNG and resize the image
    if(scalePNG > 0){
      #ifdef USE_OLED
        oledStateChange(6); //BMP to PNG
      #endif
      sprintf(pathOutput, "/output/png/%05d.png", i);
      Serial.printf("Saving PNG image in: %s",pathOutput);
      perf = millis();
      png_upscaler(fileBMPPath,pathOutput,scalePNG);
      perf = millis() - perf;
      Serial.printf("... Done! in %lums\n",perf);
      
      #ifdef LED_STATUS_PIN 
        LED_blink(LED_STATUS_PIN,1,100,50);
      #endif
      #if defined(COMMON_ANODE) || defined(COMMON_CATHODE)
        LED_blink(LED_STATUS_BLUE,1,100,50);
      #endif
    }
    
    FSYS.remove(fileBMPPath);
    Serial.printf("\n");

    //Delete the original dump files after the processing is done
    for (int z = 1; z <= numfiles; z++){
      //Check if is a single file or a long print
      if (numfiles == 1){
        sprintf(path, "/dumps/%05d.bin", i);
      }else{
        sprintf(path, "/dumps/%05d_%05d.bin", i, z);
      }
      FSYS.remove(path);
    }

    update_dumps(-1); 
  }
  
  dumpCount = get_dumps();
  // if the printer loses power or gets reset while converting,
  // the dump count can get out of sync with actual dump files
  if (dumpCount > 0) {
    Serial.printf("Dump count is out of sync! %ld dump(s) missing under /dumps\n",dumpCount);
    set_dumps(0L);
  }

  ResetPrinterVariables(); // Get Next ID available  

  //Reset Local Variables
  numfiles=0; 
  actualfile=0;
  isConverting = false;
  
  #ifdef LED_STATUS_PIN 
    LED_blink(LED_STATUS_PIN, 3,100,100);
  #endif
  #if defined(COMMON_ANODE) || defined(COMMON_CATHODE)
    LED_blink(LED_STATUS_BLUE, 3,100,100);
  #endif

  uint8_t percUsed = fs_info();
  if (percUsed <= 10) {
      full();
      delay(5000);
      ESP.restart();
  }
}

/*******************************************************************************
   Decode the Image to a 24bits BMP
*******************************************************************************/
void gbpdecoder_gotByte(const uint8_t bytgb){
  if (gbp_pkt_processByte(&gbp_pktBuff, bytgb, gbp_pktbuff, &gbp_pktbuffSize, sizeof(gbp_pktbuff))){
    if (gbp_pktBuff.received == GBP_REC_GOT_PACKET)
    {
      pktCounter++;
      if (gbp_pktBuff.command == GBP_COMMAND_PRINT)
      {
        gbp_tiles_print(&gbp_tiles,
            gbp_pktbuff[GBP_PRINT_INSTRUCT_INDEX_NUM_OF_SHEETS],
            gbp_pktbuff[GBP_PRINT_INSTRUCT_INDEX_NUM_OF_LINEFEED],
            gbp_pktbuff[GBP_PRINT_INSTRUCT_INDEX_PALETTE_VALUE],
            gbp_pktbuff[GBP_PRINT_INSTRUCT_INDEX_PRINT_DENSITY]);
            
        // Streaming BMP Writer
        // Dev Note: Done this way to allow for streaming writes to file without a large buffer
  
        // Open New File
        if(!FSYS.exists(fileBMPPath))
        {
          fileBMP = FSYS.open(fileBMPPath, "w");
          if (!fileBMP) {
            Serial.println("file creation failed");
          }
          unsigned char buf[54] = {B01000010,B01001101,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
                                  B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
                                  B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
                                  B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
                                  B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
                                  B00000000,B00000000,B00000000,B00000000};
          fileBMP.write(buf,54);
          fileBMP.close();

           // Update
          gbp_bmp.bmpSizeWidth = GBP_TILE_PIXEL_WIDTH*GBP_TILES_PER_LINE;
          gbp_bmp.bmpSizeHeight = 0;
          gbp_bmp.fileCounter++;
        }
  
         // Write Decode Data Buffer Into BMP
        for (int j = 0; j < gbp_tiles.tileRowOffset; j++)
        {
          const long int tileHeightIncrement = GBP_TILE_PIXEL_HEIGHT*GBP_BMP_MAX_TILE_HEIGHT;
          const uint8_t * bmpLineBuffer = &gbp_tiles.bmpLineBuffer[tileHeightIncrement*j][0];
          
          // Fixed width          
          if ((GBP_TILE_PIXEL_WIDTH*GBP_TILES_PER_LINE) != gbp_bmp.bmpSizeWidth)
              return;

          for (uint16_t y = 0; y < tileHeightIncrement; y++)
          {
              for (uint16_t x = 0; x < (GBP_TILE_PIXEL_WIDTH*GBP_TILES_PER_LINE); x++)
              {                  
                  const int pixel = 0b11 & (bmpLineBuffer[(y * GBP_TILE_2BIT_LINEPACK_ROWSIZE_B((GBP_TILE_PIXEL_WIDTH*GBP_TILES_PER_LINE))) + GBP_TILE_2BIT_LINEPACK_INDEX(x)] >> GBP_TILE_2BIT_LINEPACK_BITOFFSET(x));
                  const unsigned long encodedColor = palletColor[pixel & 0b11];                 
                  bmp_set(gbp_bmp.bmpBuffer, (GBP_TILE_PIXEL_WIDTH*GBP_TILES_PER_LINE), x, y, encodedColor);
              }
          }

          gbp_bmp.bmpSizeHeight += tileHeightIncrement;
                    
          fileBMP = FSYS.open(fileBMPPath, "a");
          if (!fileBMP) {
            Serial.println("file creation failed");
          }
          fileBMP.write(gbp_bmp.bmpBuffer,BMP_PIXEL_BUFF_SIZE((GBP_TILE_PIXEL_WIDTH*GBP_TILES_PER_LINE), tileHeightIncrement));
          fileBMP.close();
        }
        gbp_tiles_reset(&gbp_tiles); ///< Written to file, clear decoded tile line buffer
  
        // Print finished and cut requested        
        // Rewind and write header with the now known image size
        if (actualfile == numfiles)
        {
          bmp_header(gbp_bmp.bmpBuffer, gbp_bmp.bmpSizeWidth, gbp_bmp.bmpSizeHeight);
          
          fileBMP = FSYS.open(fileBMPPath, "r+");
          fileBMP.seek(0,SeekSet);
          fileBMP.write(gbp_bmp.bmpBuffer,54);
      
          // Close File
          fileBMP.close();          

        }
      }
    }else{
      // Support compression payload
      while (gbp_pkt_decompressor(&gbp_pktBuff, gbp_pktbuff, gbp_pktbuffSize, &tileBuff))
      {
        if (gbp_pkt_tileAccu_tileReadyCheck(&tileBuff))
        {
          // Got tile
          if (gbp_tiles_line_decoder(&gbp_tiles, tileBuff.tile))
          {
            // Line Obtained
          }
        }
      }     
    }
  }
}
