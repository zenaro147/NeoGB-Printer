
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
  Serial.println(uxTaskGetStackHighWaterMark(NULL));
  //Remove the interrupt to prevent receive data from Gameboy
  detachInterrupt(digitalPinToInterrupt(GBP_SC_PIN));

  //Clear variables
  memset(image_data, 0x00, sizeof(image_data));
  img_index = 0;

  //Create some vars to use in the process
  char path[20];
  int numfiles = 0;
  uint8_t palettebyte = 0x00;

  //Loop to check only the availables files based on nextFreeFileIndex function
  for(int i = 1; i < freeFileIndex; i++){
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
      Serial.println(path);

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
      
      //Find the Palette value present in PRINT command
      for(int bytePos=0; bytePos < img_index; bytePos++){
        if(image_data[bytePos] == B10001000 && image_data[bytePos+1] == B00110011 && image_data[bytePos+2] == B00000010){
          palettebyte = ((int)image_data[bytePos+8]);
          Serial.println(palettebyte);
          break; //After find, exit from this loop
        }
      }
      
      //Parse the palette byte found previously from int to binary (as char array)
      uint8_t bitsCount = sizeof(palettebyte) * 8;
      char str[ bitsCount + 1 ];
      uint8_t strcount = 0;
      while ( bitsCount-- )
          str[strcount++] = bitRead( palettebyte, bitsCount ) + '0';
      str[strcount] = '\0';

      //Update the palletColor with the palette values
      for(int palPos=0; palPos <= 3; palPos++){
        char resultbytes[2];
        switch (palPos) {
          case 0:
            sprintf(resultbytes, "%c%c", str[0],str[1]);
            break;
          case 1:
            sprintf(resultbytes, "%c%c", str[2],str[3]);
            break;
          case 2:
            sprintf(resultbytes, "%c%c", str[4],str[5]);
            break;
          case 3:
            sprintf(resultbytes, "%c%c", str[6],str[7]);
            break;
          default:
            break;
        }
        switch (atoi(resultbytes)) {
          case 0:
            palletColor[palPos] = {0x000000};
            break;
          case 1:
            palletColor[palPos] = {0x555555};
            break;
          case 10:
            palletColor[palPos] = {0xAAAAAA};
            break;
          case 11:
            palletColor[palPos] = {0xFFFFFF};
            break;
          default:
            break;
        }    
      }
      
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
    Serial.println(uxTaskGetStackHighWaterMark(NULL));
  }

  numfiles=0; 
  isConverting = false;
  #ifdef USE_OLED
    oled_drawSplashScreen();
  #endif

  #ifdef GBP_FEATURE_USING_RISING_CLOCK_ONLY_ISR
    attachInterrupt(digitalPinToInterrupt(GBP_SC_PIN), serialClock_ISR, RISING);  // attach interrupt handler
  #else
    attachInterrupt(digitalPinToInterrupt(GBP_SC_PIN), serialClock_ISR, CHANGE);  // attach interrupt handler
  #endif
  
  Serial.println(uxTaskGetStackHighWaterMark(NULL));
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
        /*
         * Precisa criar um arquivo BMP para receber os dados da imagem. (checa antes se o arquivo já existe, para fecha-lo         
         * ofilenameBuf - Nome do arquivo
         * GBP_TILE_PIXEL_WIDTH*GBP_TILES_PER_LINE - Define a Largura (Valores pre definidos, sempre será 8*20=160)
         * 
         * Reserva 54 bytes(espaço) no começo do arquivo
         * 
         * Atualiza a Struct gbp_bmp_t (variavel gbp_bmp) com 
         * bmpSizeWidth = 160
         * bmpSizeHeight = 0 (será atualizado no fim do processo todo)
         * soma +1 no fileCounter (acho que não irá precisar disso)
         * 
         * AINDA NÃO FECHA O ARQUIVO, SÓ FECHA NO FIM DO PROCESSO TODO... (talvez de para usar o append para escrever os dados da imagem?)
         */
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
         /*
         * Envia a Struct, uma linha de tiles, largura fixa (160), o o quanto se deve adicionar na altura, paleta de cores
         * 
         * Verifica se a Largura é igual a pre-definida (deve ser 160)
         * Loop traduz uma linha de tiles em pixels
         * Escreve no BMP
         * Soma 8 na altura
         */
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
      
          //fwrite(gbp_bmp.bmpBuffer, BMP_PIXEL_BUFF_SIZE((GBP_TILE_PIXEL_WIDTH*GBP_TILES_PER_LINE), tileHeightIncrement), 1, gbp_bmp.f);
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
          /*
           * Começa a escrever do começo do arquivo
           * Gera o cabeçalho do BMP (com altura negativa)
           * Escreve o buffer (cabeçalho fixo), com tamanho 54 bytes, 1 item, no arquivo
           * Fecha o arquivo
           */
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
