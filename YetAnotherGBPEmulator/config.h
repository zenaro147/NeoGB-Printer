//Include the SD libraries 
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#define FSYS SD

//Define the SD Card Module Pins
#define SD_CS 15
#define SD_SCK 14
#define SD_MOSI 13
#define SD_MISO 26

// Define a PushButton to use to Reset the emulator/Force merge files.
#define BTN_PUSH 34

// Total of images supported
#define MAX_IMAGES 400

//Using OLED Display
#define USE_OLED
#define OLED_SDA 21 //21
#define OLED_SCL 17 //22
// you can invert the display to easily spot the actual dimensions
// #define OLED_INVERT
// You can rotate the display by 180°
 #define OLED_ROTATE


/* Gameboy Link Cable Mapping to Arduino Pin */
// Note: Serial Clock Pin must be attached to an interrupt pin of the arduino
//  ___________
// |  6  4  2  |
//  \_5__3__1_/   (at cable)
//

// Pin Setup for ESP32 Devices
//                  | Arduino Pin | Gameboy Link Pin  |
#define GBP_VCC_PIN               // Pin 1            : 5.0V (Unused)
#define GBP_SO_PIN       23       // Pin 2            : ESP-pin 23 MOSI (Serial OUTPUT)
#define GBP_SI_PIN       19       // Pin 3            : ESP-pin 19 MISO (Serial INPUT)
#define GBP_SD_PIN                // Pin 4            : Serial Data  (Unused)
#define GBP_SC_PIN       18       // Pin 5            : ESP-pin 18 CLK  (Serial Clock)
#define GBP_GND_PIN               // Pin 6            : GND (Attach to GND Pin)
#define LED_STATUS_PIN    4       // LED blink on packet reception (2 is internal LED)
