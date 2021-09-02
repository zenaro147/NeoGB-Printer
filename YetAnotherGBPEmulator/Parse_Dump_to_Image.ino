
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
  
  memset(image_data, 0x00, sizeof(image_data));
  img_index = 0;

  char path[20];
  int x = 0;
  for(int i = 1; i < freeFileIndex; i++){
    sprintf(path, "/dumps/%05d.txt", i);
    if(FSYS.exists(path)) {
      Serial.println(i);
      Serial.println(x);
      ProcessRAWFile(i,1);
    }else{
      sprintf(path, "/dumps/%05d_%05d.txt", i, 1);
      if(FSYS.exists(path)){
        for(int z=1; z <= 99999; z++){
          sprintf(path, "/dumps/%05d_%05d.txt", i, z);
          if(!FSYS.exists(path)){
            break;
          }else{
            x++;
          }
        }
        Serial.println(i);
        Serial.println(x);
        ProcessRAWFile(i,x);
        x=0;
      }else{
        break;
      }
    }
  }

  x=0; 
  isConverting = false;
  #ifdef USE_OLED
    oled_drawSplashScreen();
  #endif

  #ifdef GBP_FEATURE_USING_RISING_CLOCK_ONLY_ISR
    attachInterrupt(digitalPinToInterrupt(GBP_SC_PIN), serialClock_ISR, RISING);  // attach interrupt handler
  #else
    attachInterrupt(digitalPinToInterrupt(GBP_SC_PIN), serialClock_ISR, CHANGE);  // attach interrupt handler
  #endif
  
  vTaskDelete(NULL);   
}

void ProcessRAWFile(int currfile, int numfiles){
  bool gotCMDPRNT = false;
  uint8_t palettebyte = 0x00;
  char path[20];

  Serial.println("Parsing...");
  Serial.println(currfile);
  Serial.println(numfiles);
      
  for (int z = 1; z <= numfiles; z++){
    if (numfiles == 1){
      sprintf(path, "/dumps/%05d.txt", currfile);
    }else{
      sprintf(path, "/dumps/%05d_%05d.txt", currfile, z);
    }    
    Serial.println(path);
    
    File file = FSYS.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
    }else{
      while(file.available()){
        image_data[img_index] = ((byte)file.read());
        img_index++;
      }
      file.close();
    }
    
    
    //Find the Palette value
    for(int x=0; x < img_index; x++){
      if(image_data[x] == B10001000 && image_data[x+1] == B00110011 && image_data[x+2] == B00000010){
        palettebyte = ((int)image_data[x+8]);
        Serial.println(palettebyte);
      }
    }
    
    //Parse the palette byte (int to binary (as char array))
    uint8_t bitsCount = sizeof(palettebyte) * 8;
    char str[ bitsCount + 1 ];
    uint8_t i = 0;
    while ( bitsCount-- )
        str[i++] = bitRead( palettebyte, bitsCount ) + '0';
    str[i] = '\0';
   Serial.println(str);

    for(int i=0; i <= 3; i++){
      char resultbytes[2];
      switch (i) {
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
          palletColor[i] = {0x000000};
          break;
        case 1:
          palletColor[i] = {0x555555};
          break;
        case 10:
          palletColor[i] = {0xAAAAAA};
          break;
        case 11:
          palletColor[i] = {0xFFFFFF};
          break;
        default:
          break;
      }    
    }
    
    //Send each byte to parse the tile
    for(int y=0; y < img_index; y++){
      //gbpdecoder_gotByte(image_data[y]);       
    }
    
    memset(image_data, 0x00, sizeof(image_data));
    img_index = 0;
    
    Serial.println("Done");
    delay(1000);
  }
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
        } 
  
        // Write Decode Data Buffer Into BMP
        for (int j = 0; j < gbp_tiles.tileRowOffset; j++)
        {
          const long int tileHeightIncrement = GBP_TILE_PIXEL_HEIGHT*GBP_BMP_MAX_TILE_HEIGHT;
          //gbp_bmp_add(&gbp_bmp, (const uint8_t *) &gbp_tiles.bmpLineBuffer[tileHeightIncrement*j][0], (GBP_TILE_PIXEL_WIDTH*GBP_TILES_PER_LINE), tileHeightIncrement, palletColor);
          /*
           * Envia a Struct, uma linha de tiles, largura fixa (160), o o quanto se deve adicionar na altura, paleta de cores
           * 
           * Verifica se a Largura é igual a pre-definida (deve ser 160)
           * Loop traduz uma linha de tiles em pixels
           * Escreve no BMP
           * Soma 8 na altura
           */
        }
        gbp_tiles_reset(&gbp_tiles); ///< Written to file, clear decoded tile line buffer
  
        // Print finished and cut requested
         if (cutPaper)
        {
          //gbp_bmp_render(&gbp_bmp);
          /*
           * Começa a escrever do começo do arquivo
           * Gera o cabeçalho do BMP (com altura negativa)
           * Escreve o buffer (cabeçalho fixo), com tamanho 54 bytes, 1 item, no arquivo
           * Fecha o arquivo
           */
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
