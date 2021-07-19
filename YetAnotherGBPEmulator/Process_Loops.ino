bool getLB = false;
bool getHB = false;
char LBbytes[256];
char HBbytes[256];
uint8_t lineReceived = 0;
uint8_t tilePerLineReceived = 0;
uint8_t tileLine[8][160];

int totalHeight = 0;

/*******************************************************************************
  Recieve Raw Data from GameBoy
*******************************************************************************/
inline void gbp_packet_capture_loop() {
  /* tiles received */
  static uint32_t byteTotal = 0;
  static uint32_t pktTotalCount = 0;
  static uint32_t pktByteIndex = 0;
  static uint16_t pktDataLength = 0;
  const size_t dataBuffCount = gbp_serial_io_dataBuff_getByteCount();
  if (
    ((pktByteIndex != 0) && (dataBuffCount > 0)) ||
    ((pktByteIndex == 0) && (dataBuffCount >= 6))
  ) {
    uint8_t data_8bit = 0;

    // Display the data payload encoded in hex
    for (int i = 0 ; i < dataBuffCount ; i++) {
      // Start of a new packet
      if (pktByteIndex == 0) {
        pktDataLength = gbp_serial_io_dataBuff_getByte_Peek(4);
        pktDataLength |= (gbp_serial_io_dataBuff_getByte_Peek(5) << 8) & 0xFF00;

        switch ((int)gbp_serial_io_dataBuff_getByte_Peek(2)) {
          case 1:
          case 2:
          case 4:
            chkHeader = (int)gbp_serial_io_dataBuff_getByte_Peek(2);
            break;
          default:
            break;
        }

#ifdef USE_OLED
        if (!isPrinting) {
          isPrinting = true;
          oled_msg("Receiving Data...");
        }
#endif

        digitalWrite(LED_STATUS_PIN, HIGH);
      }

      // Print Hex Byte
      data_8bit = gbp_serial_io_dataBuff_getByte();

      if (!isWriting) {
        if (chkHeader == 1 || chkHeader == 2 || chkHeader == 4) {
          image_data[img_index] = (byte)data_8bit;
          img_index++;
          if (chkHeader == 2 && pktByteIndex == 7) {
            cmdPRNT = (int)((char)nibbleToCharLUT[(data_8bit >> 0) & 0xF]) - '0';
          }
        }
      }

      if ((pktByteIndex > 5) && (pktByteIndex >= (9 + pktDataLength))) {
        digitalWrite(LED_STATUS_PIN, LOW);
        if (chkHeader == 2) {
          if (cmdPRNT > 0 && !isWriting) {
            gbp_serial_io_print_set();
            isWriting = true;
            xTaskCreatePinnedToCore(storeData,            // Task function.
                                    "storeData",          // name of task.
                                    10000,                // Stack size of task
                                    (void*)&image_data,   // parameter of the task
                                    1,                    // priority of the task
                                    &TaskWrite,           // Task handle to keep track of created task
                                    0);                   // pin task to core 0
          } else {
            cmdPRNT = 0x00;
            chkHeader = 99;
            isWriting = false;
            delay(200);
            gbp_serial_io_print_done();
          }
        }
        pktByteIndex = 0;
        pktTotalCount++;
      } else {
        pktByteIndex++; // Byte hex split counter
        byteTotal++; // Byte total counter
      }
    }
  }
}

/*******************************************************************************
   Decode the Image
*******************************************************************************/
void gbpdecoder_gotByte(const uint8_t bytgb){
  if (gbp_pkt_processByte(&gbp_pktBuff, bytgb, gbp_pktbuff, &gbp_pktbuffSize, sizeof(gbp_pktbuff))){
    if (gbp_pktBuff.received == GBP_REC_GOT_PACKET){
      pktCounter++;
      if (gbp_pktBuff.command == GBP_COMMAND_PRINT){
        //Save as Image File (print the last tile group)
        for (int i = 0; i < 8 ;i++){
          Serial.println(" ");
          for (int j = 0; j < 160 ;j++){
            Serial.print(tileLine[i][j]);            
            Serial.print(" ");
          }
        }
      }
    }else{
      // Support compression payload
      while (gbp_pkt_decompressor(&gbp_pktBuff, gbp_pktbuff, gbp_pktbuffSize, &tileBuff)){
        if (gbp_pkt_tileAccu_tileReadyCheck(&tileBuff)){
          // Got Tile
          for (int i = 0 ; i < GBP_TILE_SIZE_IN_BYTE ; i++){
            const uint8_t data_8bit = tileBuff.tile[i];
            parsebyte(data_8bit); 
            if(i == GBP_TILE_SIZE_IN_BYTE-1){
              //Last char of the tile
              //parsebyte(data_8bit);  
            } else {
              //Print char
              //parsebyte(data_8bit);
            }
          }
        }
      }      
    }
  }
}

void parsebyte(uint8_t abyte){
  //Convert int to binary
  uint8_t bitsCount = sizeof(abyte) * 8;
  char str[ bitsCount + 1 ];
  uint8_t i = 0;
  while ( bitsCount-- )
      str[i++] = bitRead( abyte, bitsCount ) + '0';
  str[i] = '\0';
  
  //Get High and Low Bytes
  if(!getLB && !getHB){
    strcpy(LBbytes, str);
    getLB = true;
  }else{
    if(getLB && !getHB){
      strcpy(HBbytes, str);
      getHB = true;
    }
  }

  //Process Pixels of the line
  if(getLB && getHB){

    if (lineReceived > 7){
      lineReceived = 0;
      tilePerLineReceived++;
    }
    if(tilePerLineReceived > 19){
      totalHeight = totalHeight + 8;
      tilePerLineReceived = 0;
  
      for (int i = 0; i < 8 ;i++){
        Serial.println(" ");
        for (int j = 0; j < 160 ;j++){
          Serial.print(tileLine[i][j]);            
          Serial.print(" ");
        }
      }
    }
    
    char resultbytes[2];
    for (int i = 0 ; i < 8 ; i++){
      sprintf(resultbytes, "%c%c", HBbytes[i],LBbytes[i]);
      switch (atoi(resultbytes)) {
        case 0:
          tileLine[lineReceived][i+(tilePerLineReceived*8)] = 3;
          break;
        case 1:
          tileLine[lineReceived][i+(tilePerLineReceived*8)] = 2;       
          break;
        case 10:
          tileLine[lineReceived][i+(tilePerLineReceived*8)] = 1;
          break;
        case 11:
          tileLine[lineReceived][i+(tilePerLineReceived*8)] = 0;
          break;
        default:
          break;
      }
    }

    lineReceived++;
    
    getLB = false;
    getHB = false;
    memset(LBbytes, 0x00, sizeof(LBbytes));
    memset(HBbytes, 0x00, sizeof(HBbytes));
  }  
}
