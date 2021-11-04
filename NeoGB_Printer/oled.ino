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

void oled_writeNumImages(int numTotDump, int numTotImages) { 
  char textshow[10];
  
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  sprintf(textshow, "%d Dumps", numTotDump);
  display.setCursor(56, 15);
  display.println(textshow);
  sprintf(textshow, "%d Images", numTotImages);  
  display.setCursor(56, 23);
  display.println(textshow);
  display.display();
  #ifdef OLED_INVERT
    display.invertDisplay(true);
  #endif
}


void oled_drawStatus(const unsigned char statusName[]) {
  display.clearDisplay();
  display.drawBitmap(0, 0, statusName, SCREEN_WIDTH, SCREEN_HEIGHT, 1);
  display.display();
}


void oledStateChange(uint8_t lcdStatus){
  if(prevlcdStatus != lcdStatus){
    switch (lcdStatus)
    {
      case 0:
        oled_drawStatus(splashScreen);
        break;
      case 1:      
        oled_drawStatus(idlePrinter);
        break;
      case 2:
        oled_drawStatus(sdInitError);
        break;
      case 3:
        oled_drawStatus(sdFull);
        break;
      case 4:
        oled_drawStatus(hex2txt);
        break;
      case 5:
        oled_drawStatus(txt2bmp);
        break;
      case 6:
        oled_drawStatus(txt2png);
        break;
      case 7:
        oled_drawStatus(idlePrinter2);
        break;
      case 8:
        oled_drawStatus(forceFile);
        break;
      case 99:
        oled_drawStatus(oledTest);
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
