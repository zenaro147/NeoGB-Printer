#ifdef USE_OLED

#include "./includes/display/oled_graphics.h"

#define PRINTER_ICON_HEIGHT 27
#define PRINTER_ICON_WIDTH 32

#define OLED_WHITE 0xFFFF
#define OLED_BLACK 0x0000

#ifdef USE_SSD1306 //oled images 128x32  
#include "./includes/display/SSD1306.hpp"
static LGFX_SSD1306 display; // LGFX_SSD1306のインスタンス（クラスLGFX_SSD1306を使ってlcdでいろいろできるようにする）を作成
#endif

#ifdef USE_SSD1331 //oled images 96x64
#include "./includes/display/SSD1331.hpp"
static LGFX_SSD1331 display; // LGFX_SSD1331のインスタンス（クラスLGFX_SSD1306を使ってlcdでいろいろできるようにする）を作成
#endif

uint8_t prevlcdStatus=9;

void oled_setup() {
  display.init();
  #ifdef OLED_ROTATE
    display.setRotation(2);
  #else
    display.setRotation(0);
  #endif
}

void oled_writeNumImages(int numTotDump) { 
  char textshow[15];
  
  display.setTextSize(1);
  display.setTextColor(OLED_WHITE);

  sprintf(textshow, "%d Dumps", numTotDump);

  #ifdef USE_SSD1306
  display.setCursor(get_xoffset(String(textshow)), 15);
  #endif
  #ifdef USE_SSD1331 
  display.setCursor(get_xoffset(String(textshow)), 47);
  #endif
  
  display.println(textshow);

  #ifdef USE_SSD1306
  display.setCursor(get_xoffset(String(numVersion)), 23);
  #endif
  #ifdef USE_SSD1331 
  display.setCursor(get_xoffset(String(numVersion)), 56);
  #endif

  display.println(numVersion);
}

void oled_ShowIP() { 
  display.setTextSize(1);
  display.setTextColor(OLED_WHITE);
  
  #ifdef USE_SSD1306
  display.setCursor(get_xoffset(ip), 15);
  #endif
  #ifdef USE_SSD1331 
  display.setCursor(get_xoffset(ip), 47);
  #endif
  display.println(ip);

  #ifdef USE_SSD1306
  display.setCursor(get_xoffset(mdnsName + ".local"), 23);
  #endif
  #ifdef USE_SSD1331 
  display.setCursor(get_xoffset(mdnsName + ".local"), 56);
  #endif
  display.println(mdnsName + ".local");
}

int get_xoffset(String message) {
  return ((round(SCREEN_WIDTH / 6) - message.length()) / 2) * 6;
}

void oled_drawStatus(const unsigned char statusName[]) {
  display.fillScreen(OLED_BLACK);
  display.drawBitmap(0, 0, statusName, SCREEN_WIDTH, SCREEN_HEIGHT, OLED_WHITE);
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
        oled_drawStatus(hex2bin);
        break;
      case 5:
        oled_drawStatus(bin2bmp);
        break;
      case 6:
        oled_drawStatus(bmp2png);
        break;
      case 7:
        oled_drawStatus(idlePrinter2);
        break;
      case 8:
        oled_drawStatus(forceFile);
        break;
      case 9:
        oled_drawStatus(idlePrinterServer);
        break;
      case 10:
        oled_drawStatus(bmp2bmp);
        break;
      case 11:
        oled_drawStatus(generateListFile);
        break;
      case 99:
        oled_drawStatus(oledTest);
        break;
      default:
        display.fillScreen(OLED_BLACK);
        break;
    }
    prevlcdStatus = lcdStatus;
  }
}

#endif
