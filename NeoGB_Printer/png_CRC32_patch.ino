//
// Copyright (c) 2013 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:  MIT
//

#include "./includes/image/patch_CRC32.h"

void png_patcher(char input[])
{

  //serial.print("Trying to patch: ");
  //serial.println(input);
  File file = FSYS.open(input);
  if (!file) {
    //serial.print(input);
    //serial.println(": this file does not exist");
  }

byte IEND[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82};
unsigned long IDAT_start=817;//always at the same position with PNGenc
unsigned long IDAT_end=file.size()-16;//always at the same position with PNGenc
byte data_crc;

CRC32 crc;// Calculate a checksum one byte at a time.
for (unsigned long k = IDAT_start; k < IDAT_end; k++) {
//Serial.print("Data: ");  
file.seek(k);
data_crc=file.read();
crc.update(data_crc);
//Serial.print(data_crc,HEX);
//Serial.print(" at position: ");  
//Serial.println(k,DEC); 
}
uint32_t checksum = crc.finalize();

//serial.print("Current IDAT crc: "); 
//serial.print(file.read(),HEX);
//serial.print(file.read(),HEX);
//serial.print(file.read(),HEX);
//serial.println(file.read(),HEX);
file.close();
//serial.print("Real IDAT crc: "); 
//serial.println(checksum,HEX);
//serial.println("Injecting real IDAT crc..."); 
IEND[0]=checksum>>24;
IEND[1]=checksum>>16;
IEND[2]=checksum>>8;
IEND[3]=checksum>>0;
file = FSYS.open(input,"w+");
file.seek(IDAT_end);
file.write(IEND,16);
file.close();
//serial.println("File patched !");     
}
