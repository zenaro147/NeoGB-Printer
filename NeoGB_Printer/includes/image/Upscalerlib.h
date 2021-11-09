
#include "arduino.h"
/////////////////////////////////////////////////////////////////////////////////////PNGenc stuff////////////////
#include "PNGenc.h"
// PNG encoder written by Larry Bank
// The PNG uspcaler is bugged when the crc of IDAT chunk is calculated, so there is a patch to correct this in png_CRC32_patch.ino
// Upscaler written by Raphael BOICHOT
PNG png; // static instance of the PNG encoder class
File myfile;

void * myOpen(const char *filename) {
  Serial.printf("Attempting to open %s\n", filename);
  // IMPORTANT!!! - don't use FILE_WRITE because it includes O_APPEND
  // this will cause the file to be written incorrectly
  //
  //myfile = FSYS.open(filename,O_READ | O_WRITE | O_CREAT);
  myfile = FSYS.open(filename, "w+");
  return &myfile;
}
void myClose(PNGFILE *handle) {
  File *f = (File *)handle->fHandle;
  f->close();
}
int32_t myRead(PNGFILE *handle, uint8_t *buffer, int32_t length) {
  File *f = (File *)handle->fHandle;
  return f->read(buffer, length);
}
int32_t myWrite(PNGFILE *handle, uint8_t *buffer, int32_t length) {
  File *f = (File *)handle->fHandle;
  return f->write(buffer, length);
}
int32_t mySeek(PNGFILE *handle, int32_t position) {
  File *f = (File *)handle->fHandle;
  return f->seek(position);
}

void png_upscaler(char input[], char output[], int scale_factor) {

  Serial.print("Trying to load: ");
  Serial.println(input);
  File file = FSYS.open(input);
  if (!file) {
    Serial.print(input);
    Serial.println(": this file does not exist");
  }
  Serial.println("Upscaling to indexed PNG");
  byte header[54];//read the image source header
  file.read(header, 54);
  unsigned long BMPsize = ((header[5] << 24) | (header[4] << 16) | (header[3] << 8) | (header[2]));
  unsigned long STARToffset = ((header[13] << 24) | (header[12] << 16) | (header[11] << 8) | (header[10]));
  unsigned w = ((header[21] << 24) | (header[20] << 16) | (header[19] << 8) | (header[18]));
  unsigned h = -((header[25] << 24) | (header[24] << 16) | (header[23] << 8) | (header[22]));
  unsigned up_w = w * scale_factor;
  unsigned up_h = h * scale_factor;

  unsigned WIDTH = up_w;
  unsigned HEIGHT = up_h;
  uint8_t ucPal[768] = {0x00, 0x00, 0x00, 0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA, 0xFF, 0xFF, 0xFF}; // palette entered by triplets
  //uint8_t ucAlphaPal[256] = {0,255}; // first color (black) is fully transparent
  int rc, iDataSize, x, y;
  uint8_t ucLine[WIDTH];
  byte junk;
  byte pixel;
  byte octet;
  byte normal_line[w];
  unsigned long index;

  rc = png.open(output, myOpen, myClose, myRead, myWrite, mySeek);

  if (rc == PNG_SUCCESS) {
    rc = png.encodeBegin(WIDTH, HEIGHT, PNG_PIXEL_INDEXED, 8, ucPal, 3);
    //png.setAlphaPalette(ucAlphaPal);
    if (rc == PNG_SUCCESS) {
      for (int y = 0; y < HEIGHT && rc == PNG_SUCCESS; y++) {
        // prepare a line of image to create a red box with an x on a transparent background

        index = 0;
        for (unsigned k = 0; k < w; k++) {
          pixel = file.read();
          junk = file.read();
          junk = file.read();
          if (pixel == 0xFF) octet = 0x03;
          if (pixel == 0xAA) octet = 0x02;
          if (pixel == 0x55) octet = 0x01;
          if (pixel == 0x00) octet = 0x00;
          for (unsigned i = 0; i < scale_factor; i++) {
            ucLine[index] = octet;
            index = index + 1;
          }
        }

        for (unsigned j = 0; j < scale_factor; j++) {
          rc = png.addLine(ucLine);
        }

        //Serial.println(" ");
      }
      iDataSize = png.close();
      Serial.print("PNG filesize: ");
      Serial.println(iDataSize, DEC);
    }
  } else {
    Serial.println("Failed to create the file on the SD card !");
  }
  Serial.println("Upscaled PNG file encoded !");
  file.close();
}



/*
  unsigned WIDTH=256;
  unsigned HEIGHT=256;
  uint8_t ucPal[768] = {0,0,0,0,255,0}; // black, green
  uint8_t ucAlphaPal[256] = {0,255}; // first color (black) is fully transparent
  int rc, iDataSize, x, y;
  uint8_t ucLine[WIDTH];
  rc = png.open("/testimg.png", myOpen, myClose, myRead, myWrite, mySeek);
  if (rc == PNG_SUCCESS) {
        rc = png.encodeBegin(WIDTH, HEIGHT, PNG_PIXEL_INDEXED, 8, ucPal, 3);
        png.setAlphaPalette(ucAlphaPal);
        if (rc == PNG_SUCCESS) {
            for (int y=0; y<HEIGHT && rc == PNG_SUCCESS; y++) {
              // prepare a line of image to create a red box with an x on a transparent background
              if (y==0 || y == HEIGHT-1) {
                memset(ucLine, 1, WIDTH); // top+bottom green lines
              } else {
                memset(ucLine, 0, WIDTH);
                ucLine[0] = ucLine[WIDTH-1] = 1; // left/right border
                ucLine[y] = ucLine[WIDTH-1-y] = 1; // X in the middle
              }
                rc = png.addLine(ucLine);
            } // for y
            iDataSize = png.close();
        }
  } else {
    Serial.println("Failed to create the file on the SD card!");
  }
*/


/////////////////////////////////////////////////////////////////////////////////////PNGenc stuff////////////////


  /////////////////////////////////////////////////////////////////////////////////////BMP upscaler stuff////////////////
  //create_buffer("/00001.bmp","/upscaled.bmp",4);
  void bmp_upscaler(char input[], char output[], int scale_factor) {
// BMP upscaler and compressor written by Raphael BOICHOT
  Serial.print("Trying to load: ");
  Serial.println(input);
  File file = FSYS.open(input);
  File bmp_buffer = FSYS.open(output, FILE_WRITE);
  if (!file) {
    Serial.print(input);
    Serial.println(": this file does not exist");
  }
  Serial.println("Upscaling to 4bits BMP");
  //read the image source header
  byte header[54];
  file.read(header, 54);
  //    for (unsigned x = 0; x < 54; x++) {
  //      Serial.print(header[x], HEX);
  //      Serial.print(" ");
  //    }
  unsigned long BMPsize = ((header[5] << 24) | (header[4] << 16) | (header[3] << 8) | (header[2]));
  unsigned long STARToffset = ((header[13] << 24) | (header[12] << 16) | (header[11] << 8) | (header[10]));
  unsigned w = ((header[21] << 24) | (header[20] << 16) | (header[19] << 8) | (header[18]));
  unsigned h = -((header[25] << 24) | (header[24] << 16) | (header[23] << 8) | (header[22]));
  //    Serial.println(" ");
  //    Serial.println(w, DEC);
  //    Serial.println(h, DEC);
  unsigned up_w = w * scale_factor;
  unsigned up_h = h * scale_factor;
  //    Serial.println(" ");
  //    Serial.println(up_w, DEC);
  //    Serial.println(up_h, DEC);


  unsigned long bmp_buffer_size = 54 + up_w * up_h / 2; //because 4-bits format
  //  Serial.println(bmp_buffer_size, DEC);

  unsigned char buf[118] = {0x42, 0x4D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x55, 0x00, 0xAA, 0xAA,
                            0xAA, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                           };

  buf[2] = (unsigned char)(bmp_buffer_size >>  0);
  buf[3] = (unsigned char)(bmp_buffer_size >>  8);
  buf[4] = (unsigned char)(bmp_buffer_size >> 16);
  buf[5] = (unsigned char)(bmp_buffer_size >> 24);

  //  Serial.println(buf[2], HEX);
  //  Serial.println(buf[3], HEX);

  unsigned long uw = up_w;
  unsigned long uh = -up_h;

  buf[18] = uw >>  0;
  buf[19] = uw >>  8;
  buf[20] = uw >> 16;
  buf[21] = uw >> 24;

  buf[22] = uh >>  0;
  buf[23] = uh >>  8;
  buf[24] = uh >> 16;
  buf[25] = uh >> 24;
  bmp_buffer.write(buf, 118);

  //  for (unsigned x = 0; x < 54; x++) {
  //        Serial.print(buf[x], HEX);
  //        Serial.print(" ");
  //  }

  byte junk;
  byte pixel;
  byte normal_line[up_w];
  byte compressed_line[up_w / 2]; //because 4-bits format
  byte octet;
  byte four_bits;
  unsigned long index;

  //RGB_led_ON(LED_STATUS_BLUE);
  for (unsigned m = 0; m < h; m++) {
    index = 0;
    for (unsigned k = 0; k < w; k++) {
      pixel = file.read();
      junk = file.read();
      junk = file.read();
      for (unsigned i = 0; i < scale_factor; i++) {
        normal_line[index] = pixel;
        index = index + 1;
      }
    }

    index = 0;
    for (unsigned k = 0; k < up_w / 2; k++) { //because 4-bits format
      pixel = normal_line[index];
      octet = 0;
      four_bits = 0;
      if (pixel == 0xFF) four_bits = 0x30;
      if (pixel == 0xAA) four_bits = 0x20;
      if (pixel == 0x55) four_bits = 0x10;
      if (pixel == 0x00) four_bits = 0x00;
      octet = four_bits;
      pixel = normal_line[index + 1];
      four_bits = 0;
      if (pixel == 0xFF) four_bits = 0x03;
      if (pixel == 0xAA) four_bits = 0x02;
      if (pixel == 0x55) four_bits = 0x01;
      if (pixel == 0x00) four_bits = 0x00;
      octet = octet + four_bits;
      compressed_line[k] = octet;
      index = index + 2;
    }


    //      for (unsigned x = 0; x < 100; x++) {
    //            Serial.print(normal_line[x], HEX);
    //            Serial.print(" ");
    //      }
    //       Serial.println(" ");
    //      for (unsigned x = 0; x < 100; x++) {
    //            Serial.print(compressed_line[x], HEX);
    //            Serial.print(" ");
    //      }
    //       Serial.println(" ");

    for (unsigned j = 0; j < scale_factor; j++) {
        bmp_buffer.write(compressed_line, up_w / 2);
    }
    //Serial.println(" ");
  }


  file.close();
  bmp_buffer.close();
  //RGB_led_OFF(LED_STATUS_BLUE);
  Serial.println("Upscaled BMP file encoded !");
  //Serial.println(sizeof(bmp),DEC);
  }
  /////////////////////////////////////////////////////////////////////////////////////BMP upscaler stuff////////////////
