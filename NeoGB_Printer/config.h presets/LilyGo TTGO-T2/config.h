/*********************************
 * GAMEBOY LINK CABLE DEFINITIONS
 *********************************/
// Note: Serial Clock Pin must be attached to an interrupt pin of the arduino (Pins 1 and 4 are not used)
//  ___________
// |  6  4  2  |
//  \_5__3__1_/   (at cable)
//
//#define INVERT_SERIAL_PINS //Invert pin 2 and 3 order, since the pin 2 goes o 3 in the other side of the cable, and 3 goes to 2. This is useful if you are using a breakout board for the link cable
#define ESP_MOSI_PIN 27
#define ESP_MISO_PIN 25
#define ESP_CLK_PIN 26
//#define INVERT_SERIAL_PINS //Invert pin 2 and 3 order, since the pin 2 goes o 3 in the other side of the cable, and 3 goes to 2. This is useful if you are using a breakout board for the link cable

/*****************************
 * SD CARD MODULE DEFINITIONS 
 *****************************/
//Define the SD Card Module Pins
#define SD_CS   5
#define SD_SCK  18
#define SD_MOSI 23
#define SD_MISO 19

/************************************
 * PUSH BUTTON AND IMAGE DEFINITIONS 
 ************************************/
#define BTN_PUSH 36             // Define a PushButton to use to Force a new file/Generate the images.
#define BTN_INVERT              //Define to use the LOW state instead HIGH state. Useful for the TTGO board.
#define BMP_UPSCALE_FACTOR 0    //Set the Upsace factor for the BMP output
#define PNG_UPSCALE_FACTOR 4    //Set the Upsace factor for the PNG output

/***************************
 * OLED DISPLAY DEFINITIONS
 ***************************/
#define USE_OLED        //Enable OLED

//Uncomment this if you are using SSD1306 display driver (I2C)
//#define USE_SSD1306
//#define OLED_SDA 21
//#define OLED_SCL 22
//#define OLED_COMPINS 1 // (If you have some issues with the image, try to change this with values between 1 and 4)

//Uncomment this if you are using SSD1331 display driver (SPI)
#define USE_SSD1331
#define OLED_SCLK 14
#define OLED_MOSI 13
#define OLED_MISO 12
#define OLED_CS   15
#define OLED_DC   16
#define OLED_RST   4

//Set your display dimensions
#define SCREEN_WIDTH 96
#define SCREEN_HEIGHT 64
#define OLED_ROTATE   //Rotate the display by 180°

/*************************
 * LED STATUS DEFINITIONS
 *************************/
// Standard LED Definitions
//#define LED_STATUS_PIN    5       // LED blink on packet reception (2 is internal LED)

// RGB LED Definitions
#define LED_STATUS_BLUE  2
#define LED_STATUS_RED   17
#define LED_STATUS_GREEN 21
//#define COMMON_ANODE
//#define COMMON_CATHODE

/*************************
 * WEB SERVER DEFINITIONS
 *************************/
#define ENABLE_WEBSERVER
#define WIFI_CONNECT_TIMEOUT 10000              //Connection Timeout in ms

/*************************
 * RTC DEFINITIONS
 *************************/
 #define ENABLE_RTC
 #define RTC_TIMEZONE -3 //Define your timezone
 #define RTC_TIMEDIFF 1080 //Define the time difference between the NTP server and the real time
