
#include "Arduino.h"
//SD libraries 
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#define FSYS SD

/////////////////////////////////////////////////////////////////////////////////////create the ID file////////////////
void ID_file_checker() {
  uint8_t buf[4];
  char path[]="/ID_storage.bin";
  if(FSYS.exists(path)){
    //skip the step
    Serial.println("Configuration file already on SD card");
    return;
  } else {
    File file = FSYS.open(path,FILE_WRITE);
    //start from a fresh install on SD
    unsigned long ID=1;
    buf[3] = ID >>  0;
    buf[2] = ID >>  8;
    buf[1] = ID >> 16;
    buf[0] = ID >> 24;
    file.write(buf, 4);
      
    unsigned long dumps=0;
    buf[3] = dumps >>  0;
    buf[2] = dumps >>  8;
    buf[1] = dumps >> 16;
    buf[0] = dumps >> 24;
    file.write(buf, 4);
      
//    unsigned long total_images=0;
//    buf[3] = total_images >>  0;
//    buf[2] = total_images >>  8;
//    buf[1] = total_images >> 16;
//    buf[0] = total_images >> 24;
//    file.write(buf, 4);
//    file.close();
    Serial.println("Fresh configuration file created on SD card");
  } 
}
////////////////////////////////////////////////////////////////////////////////////create the ID file////////////////

/////////////////////////////////////////////////////////////////////////////////////Get the next ID////////////////
unsigned long get_next_ID() {
  uint8_t buf[8];
  char path[]="/ID_storage.bin";
  File file = FSYS.open(path);
  file.read(buf, 8);
  unsigned long Next_ID = ((buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | (buf[3]));
  file.close();
  Serial.printf("Next Image: %05d\n",Next_ID);
  return Next_ID;
}
/////////////////////////////////////////////////////////////////////////////////////Get the next ID////////////////

////////////////////////////////////////////////////////////////////////////////////Get the dumps number////////////////
unsigned long get_dumps() {
  uint8_t buf[8];
  char path[]="/ID_storage.bin";
  File file = FSYS.open(path);
  file.read(buf, 8);
  unsigned long dumps = ((buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | (buf[7]));
  file.close();
  return dumps;
}
/////////////////////////////////////////////////////////////////////////////////////Get the dumps number////////////////

////////////////////////////////////////////////////////////////////////////////////Get the total image number////////////////
//unsigned long get_images() {
//  uint8_t buf[12];
//  char path[]="/ID_storage.bin";
//  File file = FSYS.open(path);
//  file.read(buf, 12);
//  unsigned long images = ((buf[8] << 24) | (buf[9] << 16) | (buf[10] << 8) | (buf[11]));
//  file.close();
//  return images;
//}
/////////////////////////////////////////////////////////////////////////////////////Get the total image number////////////////

/////////////////////////////////////////////////////////////////////////////////////update the next ID////////////////
void update_next_ID(int mod) {
  uint8_t buf[8];
  char path[]="/ID_storage.bin";
  File file = FSYS.open(path);
  file.read(buf, 8);
  unsigned long Next_ID = ((buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | (buf[3]));
  Next_ID=Next_ID+mod;
  if (Next_ID>99999) Next_ID=1;
  file.close();
  buf[3] = Next_ID >>  0;
  buf[2] = Next_ID >>  8;
  buf[1] = Next_ID >> 16;
  buf[0] = Next_ID >> 24;
  file = FSYS.open(path,FILE_WRITE);
  file.write(buf, 8);
  file.close();
}
unsigned long update_get_next_ID(int mod) {
  uint8_t buf[8];
  char path[]="/ID_storage.bin";
  File file = FSYS.open(path);
  file.read(buf, 8);
  unsigned long Next_ID = ((buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | (buf[3]));
  Next_ID=Next_ID+mod;
  if (Next_ID>99999) Next_ID=1;
  file.close();
  buf[3] = Next_ID >>  0;
  buf[2] = Next_ID >>  8;
  buf[1] = Next_ID >> 16;
  buf[0] = Next_ID >> 24;
  file = FSYS.open(path,FILE_WRITE);
  file.write(buf, 8);
  file.close();
  Serial.printf("Next Image: %05d\n",Next_ID);
  return Next_ID;
}
/////////////////////////////////////////////////////////////////////////////////////update the next ID////////////////

/////////////////////////////////////////////////////////////////////////////////////update the dumps////////////////
void update_dumps(int mod) {
  uint8_t buf[8];
  char path[]="/ID_storage.bin";
  File file = FSYS.open(path);
  file.read(buf, 8);
  unsigned long dumps = ((buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | (buf[7]));
  dumps=dumps+mod;
  file.close();
  buf[7] = dumps >>  0;
  buf[6] = dumps >>  8;
  buf[5] = dumps >> 16;
  buf[4] = dumps >> 24;
  file = FSYS.open(path,FILE_WRITE);
  file.write(buf, 8);
  file.close();
}
unsigned long update_get_dumps(int mod) {
  uint8_t buf[8];
  char path[]="/ID_storage.bin";
  File file = FSYS.open(path);
  file.read(buf, 8);
  unsigned long dumps = ((buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | (buf[7]));
  dumps=dumps+mod;
  file.close();
  buf[7] = dumps >>  0;
  buf[6] = dumps >>  8;
  buf[5] = dumps >> 16;
  buf[4] = dumps >> 24;
  file = FSYS.open(path,FILE_WRITE);
  file.write(buf, 8);
  file.close();
  return dumps;
}
void set_dumps(long dumps) {
  uint8_t buf[8];
  char path[]="/ID_storage.bin";
  File file = FSYS.open(path);
  file.read(buf, 8);
  file.close();
  buf[7] = dumps >>  0;
  buf[6] = dumps >>  8;
  buf[5] = dumps >> 16;
  buf[4] = dumps >> 24;
  file = FSYS.open(path,FILE_WRITE);
  file.write(buf, 8);
  file.close();
}
/////////////////////////////////////////////////////////////////////////////////////update the dumps////////////////

/////////////////////////////////////////////////////////////////////////////////////update the total images////////////////
//void update_images(int mod) {
//  uint8_t buf[12];
//  char path[]="/ID_storage.bin";
//  File file = FSYS.open(path);
//  file.read(buf, 12);
//  unsigned long images = ((buf[8] << 24) | (buf[9] << 16) | (buf[10] << 8) | (buf[11]));
//  images=images+mod;
//  file.close();
//  buf[11] = images >>  0;
//  buf[10] = images >>  8;
//  buf[9] = images >> 16;
//  buf[8] = images >> 24;
//  file = FSYS.open(path,FILE_WRITE);
//  file.write(buf, 12);
//  file.close();
//}
//unsigned long update_get_images(int mod) {
//  uint8_t buf[12];
//  char path[]="/ID_storage.bin";
//  File file = FSYS.open(path);
//  file.read(buf, 12);
//  unsigned long images = ((buf[8] << 24) | (buf[9] << 16) | (buf[10] << 8) | (buf[11]));
//  images=images+mod;
//  file.close();
//  buf[11] = images >>  0;
//  buf[10] = images >>  8;
//  buf[9] = images >> 16;
//  buf[8] = images >> 24;
//  file = FSYS.open(path,FILE_WRITE);
//  file.write(buf, 12);
//  file.close();
//  return images;
//}
/////////////////////////////////////////////////////////////////////////////////////update the total images////////////////
