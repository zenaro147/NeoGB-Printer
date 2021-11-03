//lodepng support adapted by Raphaël BOICHOT 2021-10-24 from:
//https://raw.githubusercontent.com/lvandeve/lodepng/master/examples/example_bmp2png.cpp

/*
  LodePNG Examples

  Copyright (c) 2005-2010 Lode Vandevenne

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
*/

#include "arduino.h"
#include "lodepng.h"

//create_buffer("/00001.bmp","/upscaled.bmp",4);
void create_buffer(char input[], char output[], int scale_factor) {

  Serial.print("Trying to load: ");
  Serial.println(input);

  File file = FSYS.open(input);
  File bmp_buffer = FSYS.open(output, FILE_WRITE);
  if (!file) {
    Serial.print(input);
    Serial.println(": this file does not exist");
  }

  //read the image source header
  byte header[54];
  file.read(header, 54);
//  for (unsigned x = 0; x < 54; x++) {
//    Serial.print(header[x], HEX);
//    Serial.print(" ");
//  }
  unsigned long BMPsize = ((header[5] << 24) | (header[4] << 16) | (header[3] << 8) | (header[2]));
  unsigned long STARToffset = ((header[13] << 24) | (header[12] << 16) | (header[11] << 8) | (header[10]));
  unsigned w = ((header[21] << 24) | (header[20] << 16) | (header[19] << 8) | (header[18]));
  unsigned h = -((header[25] << 24) | (header[24] << 16) | (header[23] << 8) | (header[22]));
//  Serial.println(" ");
//  Serial.println(w, DEC);
//  Serial.println(h, DEC);
  unsigned up_w = w * scale_factor;
  unsigned up_h = h * scale_factor;
//  Serial.println(" ");
//  Serial.println(up_w, DEC);
//  Serial.println(up_h, DEC);


  unsigned long bmp_buffer_size = 54 + up_w*up_h/2;//because 4-bits format
  //  Serial.println(bmp_buffer_size, DEC);

  unsigned char buf[118] = {0x42, 0x4D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x55, 0x00, 0xAA, 0xAA,
                            0xAA, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  buf[2] = (unsigned char)(bmp_buffer_size >>  0);
  buf[3] = (unsigned char)(bmp_buffer_size >>  8);
  buf[4] = (unsigned char)(bmp_buffer_size >> 16);
  buf[5] = (unsigned char)(bmp_buffer_size >> 24);

//  Serial.println(buf[2], HEX);
//  Serial.println(buf[3], HEX);

  unsigned long uw = up_w;
  unsigned long uh = -up_h;
  /* biWidth */
  buf[18] = uw >>  0;
  buf[19] = uw >>  8;
  buf[20] = uw >> 16;
  buf[21] = uw >> 24;

  /* biHeight */
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
  byte compressed_line[up_w/2];//because 4-bits format
  byte octet;
  byte four_bits;
  unsigned long index;

//RGB_led_ON(LED_STATUS_BLUE);
  for (unsigned m = 0; m < h; m++) {
  index=0;
    for (unsigned k = 0; k < w; k++) {
      pixel = file.read();
      junk = file.read();
      junk = file.read();
      for (unsigned i = 0; i < scale_factor; i++) {
      normal_line[index] = pixel;
      index=index+1;
      }
    }

    index=0;
    for (unsigned k = 0; k < up_w/2; k++) {//because 4-bits format
      pixel=normal_line[index];
      octet = 0;
      four_bits = 0;
      if (pixel == 0xFF) four_bits = 0x30;
      if (pixel == 0xAA) four_bits = 0x20;
      if (pixel == 0x55) four_bits = 0x10;
      if (pixel == 0x00) four_bits = 0x00;
      octet = four_bits;
      pixel=normal_line[index+1];
      four_bits = 0;
      if (pixel == 0xFF) four_bits = 0x03;
      if (pixel == 0xAA) four_bits = 0x02;
      if (pixel == 0x55) four_bits = 0x01;
      if (pixel == 0x00) four_bits = 0x00;
      octet = octet + four_bits;
      compressed_line[k] = octet;
      index=index+2;
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
      for (unsigned k = 0; k < up_w/2 ; k++) {//because 4-bits format
          bmp_buffer.write(compressed_line[k]);
          //Serial.print(normal_line[k], HEX);
      }
      //Serial.println(" ");
    }
    //Serial.println(" ");
  }


  file.close();
  bmp_buffer.close();
//RGB_led_OFF(LED_STATUS_BLUE);
  Serial.println("Upscaled BMP created");
  //Serial.println(sizeof(bmp),DEC);
}


unsigned decodeBMP(std::vector<unsigned char>& image, unsigned& w, unsigned& h, const std::vector<unsigned char>& bmp) {
  static const unsigned MINHEADER = 54; //minimum BMP header size

  if (bmp.size() < MINHEADER) Serial.println("File shorter than the minimum BMP header size");

  //  Serial.print("Size of the BMP file: ");
  //  Serial.println(bmp.size(),DEC);

  if (bmp[0] != 'B' || bmp[1] != 'M') Serial.println("It's not a BMP file if it doesn't start with marker 'BM'");
  unsigned pixeloffset = bmp[10] + 256 * bmp[11]; //where the pixel data starts
  //read width and height from BMP header
  w = bmp[18] + bmp[19] * 256;
  h = 0xFFFF - (bmp[22] + bmp[23] * 256) + 1;

  //read number of channels from BMP header
  if (bmp[28] != 4) Serial.println("Only 4-bit BMPs are supported !");
  //Game Boy Printer images are always 160*xxx, so width multiple of 4 and w=160
  unsigned dataSize = w * h / 2; //bmp image here contains always 2 pixels per bytes
  if (bmp.size() < dataSize + pixeloffset) Serial.println("BMP file too small to contain all pixels !");

  //  Serial.print("bmp[28], sise of data in bits for a pixel: ");
  //  Serial.println(bmp[28], DEC);
  //  Serial.print("Size of the BMP file: ");
  //  Serial.println(bmp.size(), DEC);
  //  Serial.print("dataSize + pixeloffset: ");
  //  Serial.println(dataSize + pixeloffset, DEC);
  //  Serial.print("dataSize: ");
  //  Serial.println(dataSize, DEC);
  //  Serial.print("Pixeloffset: ");
  //  Serial.println(pixeloffset, DEC);
  //  Serial.print("w: ");
  //  Serial.println(w, DEC);
  //  Serial.print("h: ");
  //  Serial.println(h, DEC);
  //  Serial.print("w * h: ");
  //  Serial.println(w * h, DEC);

  image.resize(w * h / 2); //bmp contains 2 pixels per bytes
  unsigned starting_BMP_offset = pixeloffset;

  //  Serial.print("starting_BMP_offset: ");
  //  Serial.println(starting_BMP_offset, DEC);

  unsigned IMAGE_offset = 0;
  unsigned bmpos = starting_BMP_offset;
  byte pixel_1_input;
  byte pixel_2_input;
  byte dual_byte_output;
  bool p;
  for (unsigned y = h; y > 0; y--) {
    for (unsigned x = 0; x < w / 4; x++) { //bmp contains 2 pixels per bytes so w/2
      dual_byte_output = 0;
      pixel_1_input = bmp[bmpos];
      pixel_2_input = bmp[bmpos + 1];
      p = bitRead(pixel_1_input, 5);
      bitWrite(dual_byte_output, 7, p);
      p = bitRead(pixel_1_input, 4);
      bitWrite(dual_byte_output, 6, p);
      p = bitRead(pixel_1_input, 1);
      bitWrite(dual_byte_output, 5, p);
      p = bitRead(pixel_1_input, 0);
      bitWrite(dual_byte_output, 4, p);

      p = bitRead(pixel_2_input, 5);
      bitWrite(dual_byte_output, 3, p);
      p = bitRead(pixel_2_input, 4);
      bitWrite(dual_byte_output, 2, p);
      p = bitRead(pixel_2_input, 1);
      bitWrite(dual_byte_output, 1, p);
      p = bitRead(pixel_2_input, 0);
      bitWrite(dual_byte_output, 0, p);

      image[IMAGE_offset] = dual_byte_output;
      bmpos = bmpos + 2;
      IMAGE_offset = IMAGE_offset + 1;
    }
    //bmpos = bmpos - w; //shift two lines up (one line from starting offset)
  }
  return 0;
}

//typical syntax: bmp_2_png_encoder("/sd/00001.bmp","/sd/00001.png");
void bmp_2_png_encoder(char input[], char output[]) {

  std::vector<unsigned char> bmp;
  lodepng::load_file(bmp, input);
  unsigned error = lodepng::load_file(bmp, input);
  if (error) Serial.println(lodepng_error_text(error));
  if (error == 0) Serial.println("BMP loaded with success");
  std::vector<unsigned char> image;
  unsigned w, h;
  error = decodeBMP(image, w, h, bmp);
  if (error == 0) Serial.println("BMP decoded with success");
  // syntax from https://github.com/tidklaas/esp32-gameboy-printer
  lodepng::State state;
  state.info_raw.colortype = LCT_GREY;
  state.info_raw.bitdepth = 2;
  state.info_png.color.colortype = LCT_GREY;
  state.info_png.color.bitdepth = 2;
  state.encoder.zlibsettings.btype = 0;
  std::vector<unsigned char> png;
  error = lodepng::encode(png, image, w, h, state);
  if (error) Serial.println(lodepng_error_text(error));
  if (error == 0) Serial.println("PNG file encoded");
  error = lodepng::save_file(png, output);
  if (error) Serial.println(lodepng_error_text(error));
  if (error == 0) Serial.println("PNG file saved");
}
