#ifdef USE_OLED

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "oled_graphics.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#ifndef OLED_SDA
#define OLED_SDA 21
#endif
#ifndef OLED_SCL
#define OLED_SCL 22
#endif

#define PRINTER_ICON_HEIGHT 27
#define PRINTER_ICON_WIDTH 32

uint8_t prevlcdStatus=9;

void oled_setup() {
  Wire.begin(OLED_SDA, OLED_SCL);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  #ifdef OLED_ROTATE
    display.setRotation(2);
  #endif
}

void oled_msg(String message) {
  display.clearDisplay();
  oled_msg(message, 12);
}

void oled_msg(String message1, String message2) {
  display.clearDisplay();
  oled_msg(message1 + "\n" + message2, 8);
}

void oled_msg(String message1, String message2, String message3) {
  display.clearDisplay();
  oled_msg(message1 + "\n" + message2 + "\n" + message3, 4);
}

void oled_msg(String message1, String message2, String message3, String message4) {
  display.clearDisplay();
  oled_msg(message1 + "\n" + message2 + "\n" + message3 + "\n" + message4, 0);
}

void oled_msg(String message, byte y) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, y);
  display.println(message);
  display.display();
  #ifdef OLED_INVERT
    display.invertDisplay(true);
  #endif
}


void oled_drawSplashScreen() {
  display.clearDisplay();
  display.drawBitmap(0, 0, splashScreen, SCREEN_WIDTH, SCREEN_HEIGHT, 1);
  display.display();
}

void oled_drawSdInitError() {
  display.clearDisplay();
  display.drawBitmap(0, 0, sdInitError, SCREEN_WIDTH, SCREEN_HEIGHT, 1);
  display.display();
}

void oled_drawSdFull() {
  display.clearDisplay();
  display.drawBitmap(0, 0, sdFull, SCREEN_WIDTH, SCREEN_HEIGHT, 1);
  display.display();
}

void oled_drawIdleScreen() {
  display.clearDisplay();
  display.drawBitmap(0, 0, idlePrinter, SCREEN_WIDTH, SCREEN_HEIGHT, 1);
  display.display();
}

void oled_drawHex2Txt() {
  display.clearDisplay();
  display.drawBitmap(0, 0, hex2txt, SCREEN_WIDTH, SCREEN_HEIGHT, 1);
  display.display();
}

void oled_drawTxt2Bmp() {
  display.clearDisplay();
  display.drawBitmap(0, 0, txt2bmp, SCREEN_WIDTH, SCREEN_HEIGHT, 1);
  display.display();
}

void oled_drawTxt2Png() {
  display.clearDisplay();
  display.drawBitmap(0, 0, txt2png, SCREEN_WIDTH, SCREEN_HEIGHT, 1);
  display.display();
}

void oled_drawUpsalingImage() {
  display.clearDisplay();
  display.drawBitmap(0, 0, upscalingImage, SCREEN_WIDTH, SCREEN_HEIGHT, 1);
  display.display();
}

void oled_drawForceNewFile() {
  display.clearDisplay();
  display.drawBitmap(0, 0, forceFile, SCREEN_WIDTH, SCREEN_HEIGHT, 1);
  display.display();
}

void oled_drawTestOlED() {
  display.clearDisplay();
  display.drawBitmap(0, 0, oledTest, SCREEN_WIDTH, SCREEN_HEIGHT, 1);
  display.display();
}


void oledStateChange(uint8_t lcdStatus){
  if(prevlcdStatus != lcdStatus){
    switch (lcdStatus)
    {
      case 0:
        oled_drawSplashScreen();
        break;
      case 1:
        oled_drawIdleScreen();
        break;
      case 2:
        oled_drawSdInitError();
        break;
      case 3:
        oled_drawSdFull();
        break;
      case 4:
        oled_drawHex2Txt();
        break;
      case 5:
        oled_drawTxt2Bmp();
        break;
      case 6:
        oled_drawTxt2Png();
        break;
      case 7:
        oled_drawUpsalingImage();
        break;
      case 8:
        oled_drawForceNewFile();
        break;
      case 99:
        oled_drawTestOlED();
        break;
      default:
        display.clearDisplay();
        break;
    }
    #ifdef OLED_INVERT
      display.invertDisplay(true);
    #endif
    prevlcdStatus = lcdStatus;
  }
}

#endif
