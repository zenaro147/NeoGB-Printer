
#include "gbp_tiles.h"
#include "gbp_bmp.h"

uint8_t pktCounter = 0; // Dev Varible

uint32_t palletColor[4] = {0xFFFFFF,0xAAAAAA,0x555555,0x000000};

uint8_t gbp_pktbuffSize = 0;
gbp_pkt_tileAcc_t tileBuff = {0};
gbp_tile_t gbp_tiles = {0};
gbp_bmp_t  gbp_bmp = {0};

static void gbpdecoder_gotByte(const uint8_t bytgb);

/*******************************************************************************
  Convert to BMP
*******************************************************************************/
void ConvertFilesBMP(void *pvParameters)
{
  detachInterrupt(digitalPinToInterrupt(GBP_SC_PIN));
  
  byte ch = 0;
  bool skipLine = false;
  int  lowNibFound = 0;
  uint8_t byt = 0;
  unsigned int bytec = 0;

  File root = FSYS.open("/dumps");
  File filedir = root.openNextFile();
  while (filedir) {
    if (!filedir.isDirectory()) {
      char path[14];
      sprintf(path, "/dumps/%s", filedir.name());

      File file = FSYS.open(path);
      while (file.available())
      {
        gbpdecoder_gotByte((byte)file.read());
      }
      file.close();
      
      Serial.println("Done");
    }
    filedir = root.openNextFile();
  }

  #ifdef GBP_FEATURE_USING_RISING_CLOCK_ONLY_ISR
    attachInterrupt(digitalPinToInterrupt(GBP_SC_PIN), serialClock_ISR, RISING);  // attach interrupt handler
  #else
    attachInterrupt(digitalPinToInterrupt(GBP_SC_PIN), serialClock_ISR, CHANGE);  // attach interrupt handler
  #endif
  
  isConverting = false;
  #ifdef USE_OLED
    oled_drawSplashScreen();
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
        if (!gbp_bmp_isopen(&gbp_bmp))
        {
          //gbp_bmp_open(&gbp_bmp, ofilenameBuf, GBP_TILE_PIXEL_WIDTH*GBP_TILES_PER_LINE);
        } 
  
        // Write Decode Data Buffer Into BMP
        for (int j = 0; j < gbp_tiles.tileRowOffset; j++)
        {
          const long int tileHeightIncrement = GBP_TILE_PIXEL_HEIGHT*GBP_BMP_MAX_TILE_HEIGHT;
          //gbp_bmp_add(&gbp_bmp, (const uint8_t *) &gbp_tiles.bmpLineBuffer[tileHeightIncrement*j][0], (GBP_TILE_PIXEL_WIDTH*GBP_TILES_PER_LINE), tileHeightIncrement, palletColor);
        }
        gbp_tiles_reset(&gbp_tiles); ///< Written to file, clear decoded tile line buffer
  
        // Print finished and cut requested
         if (cutPaper)
        {
          //gbp_bmp_render(&gbp_bmp);
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
