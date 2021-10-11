
#include "gbp_tiles.h"
#include "gbp_bmp.h"
#include "image\bmp_FixedWidthStream.h"

uint8_t pktCounter = 0; // Dev Varible

uint32_t palletColor[4] = {0xFFFFFF,0xAAAAAA,0x555555,0x000000};

uint8_t gbp_pktbuffSize = 0;
gbp_pkt_tileAcc_t tileBuff = {0};
gbp_tile_t gbp_tiles = {0};
gbp_bmp_t  gbp_bmp = {0};

static void gbpdecoder_gotByte(const uint8_t bytgb);

File fileBMP;
char fileBMPPath[40];

/*******************************************************************************
  Convert to BMP
*******************************************************************************/
void ConvertFilesBMP(void *pvParameters)
{
  //Remove the interrupt to prevent receive data from Gameboy
  detachInterrupt(digitalPinToInterrupt(GBP_SC_PIN));

  //Clear variables
  memset(image_data, 0x00, sizeof(image_data));
  img_index = 0;

  //Create some vars to use in the process
  char path[20];
  int numfiles = 0;

  //Get the first dump ID
  char pathcheck1[20];
  char pathcheck2[20];
  int firstDumpID = 1;
  do {
    sprintf(pathcheck1, "/dumps/%05d.txt", firstDumpID);
    sprintf(pathcheck2, "/dumps/%05d_%05d.txt", firstDumpID, 1);
    if (FSYS.exists(pathcheck1) || FSYS.exists(pathcheck2)) {
      break;
    }
    firstDumpID++;
  } while(true);
  
  //Loop to check only the availables files based on nextFreeFileIndex function
  for(int i = firstDumpID; i < freeFileIndex; i++){
    //Check if the file is a long print or a single file
    sprintf(path, "/dumps/%05d.txt", i);
    if(FSYS.exists(path)) {
      numfiles=1;
    }else{     
      //If is long print, count how many files exists
      sprintf(path, "/dumps/%05d_%05d.txt", i, 1);
      if(FSYS.exists(path)){
        for(int filecount=1; filecount <= 100; filecount++){
          sprintf(path, "/dumps/%05d_%05d.txt", i, filecount);
          if(!FSYS.exists(path)){
            break; //Exit from the loop to check the ammount of files in a multiprint
          }else{
            numfiles++;
          }
        }
      }else{
        break; //Exit from the loop, because don't have any more files
      }
    }

    // Loop to parse the files based on the previous results
    for (int z = 1; z <= numfiles; z++){
      //Check if is a single file or a long print
      if (numfiles == 1){
        sprintf(path, "/dumps/%05d.txt", i);
      }else{
        sprintf(path, "/dumps/%05d_%05d.txt", i, z);
      }
      sprintf(fileBMPPath, "/output/%05d.bmp", i);

      
      Serial.print("Parsing File: ");
      Serial.print(path);
      Serial.print(" to ");
      Serial.println(fileBMPPath);

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
      FSYS.remove(path);
           
      //Send each byte to parse the tile
      for(int bytePos=0; bytePos < img_index; bytePos++){
        gbpdecoder_gotByte(image_data[bytePos]);       
      }

      //clear the variables
      memset(image_data, 0x00, sizeof(image_data));
      img_index = 0;
      
      Serial.println("Done");
      delay(1000);
    }
    //Reset the counter for the number of files 
    numfiles=0;
  }

  numfiles=0; 
  isConverting = false;
  
  callNextFile();
  resetValues();
  
  #ifdef GBP_FEATURE_USING_RISING_CLOCK_ONLY_ISR
    attachInterrupt(digitalPinToInterrupt(GBP_SC_PIN), serialClock_ISR, RISING);  // attach interrupt handler
  #else
    attachInterrupt(digitalPinToInterrupt(GBP_SC_PIN), serialClock_ISR, CHANGE);  // attach interrupt handler
  #endif
  
  #ifdef USE_OLED
    delay(1000);
    oledStateChange(1); //Printer Idle
    delay(1000);
  #endif
  
  vTaskDelete(NULL);   
}
  
/*******************************************************************************
   Decode the Image
*******************************************************************************/
void gbpdecoder_gotByte(const uint8_t bytgb){
  if (gbp_pkt_processByte(&gbp_pktBuff, bytgb, gbp_pktbuff, &gbp_pktbuffSize, sizeof(gbp_pktbuff))){
    if (gbp_pktBuff.received == GBP_REC_GOT_PACKET)
    {
      pktCounter++;
      if (gbp_pktBuff.command == GBP_COMMAND_PRINT)
      {
        const bool cutPaper = ((gbp_pktbuff[GBP_PRINT_INSTRUCT_INDEX_NUM_OF_LINEFEED]&0xF) != 0) ? true : false;  ///< if lower margin is zero, then new pic
        
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
                  //printf("bmp_set %d, %d\r\n", x, y);
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
        if (cutPaper)
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
           // Per Line Decoded (Pre Pallet Harmonisation)
            for (int j = 0; j < GBP_TILE_PIXEL_HEIGHT; j++)
            {
              for (int i = 0; i < (GBP_TILE_PIXEL_WIDTH * GBP_TILES_PER_LINE); i++)
              {
                int pixel = 0b11 & (gbp_tiles.bmpLineBuffer[j+(gbp_tiles.tileRowOffset-1)*8][GBP_TILE_2BIT_LINEPACK_INDEX(i)] >> GBP_TILE_2BIT_LINEPACK_BITOFFSET(i));;
                int b = 0;
                switch (pixel)
                {
                  case 0: b = 0; break;
                  case 1: b = 64; break;
                  case 2: b = 130; break;
                  case 3: b = 255; break;
                }
              }
            }
          }
        }
      }     
    }
  }
}
