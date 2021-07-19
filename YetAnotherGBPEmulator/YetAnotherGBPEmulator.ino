#include <stdint.h> // uint8_t
#include <stddef.h> // size_t

#include "gbp_emulator_config.h"
#include "gameboy_printer_protocol.h"
#include "gbp_pkt.h"

#include "gbp_serial_io.h"

/*******************************************************************************
*******************************************************************************/

// Dev Note: Gamboy camera sends data payload of 640 bytes usually
#define GBP_BUFFER_SIZE 650

/* Serial IO */
// This circular buffer contains a stream of raw packets from the gameboy
uint8_t gbp_serialIO_raw_buffer[GBP_BUFFER_SIZE] = {0};
inline void gbp_packet_capture_loop();

/*******************************************************************************
   Custom Variables
*******************************************************************************/
// Other Variables
uint8_t pktCounter = 0; // Dev Varible
gbp_pkt_t gbp_pktBuff = {GBP_REC_NONE, 0};
uint8_t gbp_pktbuff[GBP_PKT_PAYLOAD_BUFF_SIZE_IN_BYTE] = {0};
uint8_t gbp_pktbuffSize = 0;
gbp_pkt_tileAcc_t tileBuff = {0};

// Pallet
uint32_t palletColor[4] = {0xFFFFFF, 0xAAAAAA, 0x555555, 0x000000};

static void gbpdecoder_gotByte(const uint8_t bytgb);

/*******************************************************************************
   Custom Variables
*******************************************************************************/
uint8_t chkHeader = 99;
uint8_t cmdPRNT = 0x00;
bool isPrinting = false;
bool isWriting = false;
bool isConverting = false;
bool isFileMounted = false;
unsigned int nextFreeFileIndex();
unsigned int freeFileIndex = 0;

const char nibbleToCharLUT[] = "0123456789ABCDEF";
byte image_data[83000] = {}; //moreless 14 photos (82.236)
uint32_t img_index = 0x00;

bool isShowingSplash = false;

SPIClass spiSD(HSPI);
TaskHandle_t TaskWrite;

/*******************************************************************************
  Utility Functions
*******************************************************************************/
const char *gbpCommand_toStr(int val)
{
  switch (val)
  {
    case GBP_COMMAND_INIT    : return "INIT";
    case GBP_COMMAND_PRINT   : return "PRNT";
    case GBP_COMMAND_DATA    : return "DATA";
    case GBP_COMMAND_BREAK   : return "BREK";
    case GBP_COMMAND_INQUIRY : return "INQY";
    default: return "?";
  }
}

/*******************************************************************************
  Interrupt Service Routine
*******************************************************************************/
void ICACHE_RAM_ATTR serialClock_ISR(void)
{
  // Serial Clock (1 = Rising Edge) (0 = Falling Edge); Master Output Slave Input (This device is slave)
#ifdef GBP_FEATURE_USING_RISING_CLOCK_ONLY_ISR
  const bool txBit = gpb_serial_io_OnRising_ISR(digitalRead(GBP_SO_PIN));
#else
  const bool txBit = gpb_serial_io_OnChange_ISR(digitalRead(GBP_SC_PIN), digitalRead(GBP_SO_PIN));
#endif
  digitalWrite(GBP_SI_PIN, txBit ? HIGH : LOW);
}

/*******************************************************************************
  Reset Variables
*******************************************************************************/
void resetValues() {
  memset(image_data, 0x00, sizeof(image_data));
  img_index = 0x00;

  // Turn LED ON
  digitalWrite(LED_STATUS_PIN, false);
  Serial.println("Printer ready.");

  cmdPRNT = 0x00;
  chkHeader = 99;
  isPrinting = false;
  isWriting = false;
}

/*******************************************************************************
  Main Setup and Loop
*******************************************************************************/
void setup(void)
{
  // Config Serial
  // Has to be fast or it will not transfer the image fast enough to the computer
  Serial.begin(115200);

  /* Setup File System and OLED*/
#ifdef USE_OLED
  oled_setup();
#endif
  fs_setup();
  freeFileIndex = nextFreeFileIndex();

  /* Pins from gameboy link cable */
  pinMode(GBP_SC_PIN, INPUT);
  pinMode(GBP_SO_PIN, INPUT);
  pinMode(GBP_SI_PIN, OUTPUT);

  /* Default link serial out pin state */
  digitalWrite(GBP_SI_PIN, LOW);

  /* LED Indicator */
  pinMode(LED_STATUS_PIN, OUTPUT);
  digitalWrite(LED_STATUS_PIN, LOW);

  /* Pin for pushbutton */
  pinMode(BTN_CONVERT, INPUT);

  /* Setup */
  gpb_serial_io_init(sizeof(gbp_serialIO_raw_buffer), gbp_serialIO_raw_buffer);

  /* Attach ISR */
#ifdef GBP_FEATURE_USING_RISING_CLOCK_ONLY_ISR
  attachInterrupt(digitalPinToInterrupt(GBP_SC_PIN), serialClock_ISR, RISING);  // attach interrupt handler
#else
  attachInterrupt(digitalPinToInterrupt(GBP_SC_PIN), serialClock_ISR, CHANGE);  // attach interrupt handler
#endif

  gbp_pkt_init(&gbp_pktBuff);
}

void loop()
{
  gbp_packet_capture_loop();

  // Trigger Timeout and reset the printer if byte stopped being received.
  static uint32_t last_millis = 0;
  uint32_t curr_millis = millis();
  if (curr_millis > last_millis)
  {
    uint32_t elapsed_ms = curr_millis - last_millis;
    if (gbp_serial_io_timeout_handler(elapsed_ms))
    {

      Serial.print ("Timeout Printer");
      // Timeout code
      digitalWrite(LED_STATUS_PIN, LOW);
#ifdef USE_OLED
      if (!isShowingSplash) {
        isShowingSplash = true;
        oled_drawSplashScreen();
      }
#endif
    }
  }
  last_millis = curr_millis;


// Check Button Press
  if (digitalRead(BTN_CONVERT) == HIGH && !isConverting && !isWriting) {
    isConverting = true;
    detachInterrupt(digitalPinToInterrupt(GBP_SC_PIN));
#ifdef USE_OLED
    oled_msg("Saving BMP Image...");
#endif

    ConvertFilesBMP();

#ifdef GBP_FEATURE_USING_RISING_CLOCK_ONLY_ISR
    attachInterrupt(digitalPinToInterrupt(GBP_SC_PIN), serialClock_ISR, RISING);  // attach interrupt handler
#else
    attachInterrupt(digitalPinToInterrupt(GBP_SC_PIN), serialClock_ISR, CHANGE);  // attach interrupt handler
#endif

    isConverting = false;
  }

    // Diagnostics Console
  while (Serial.available() > 0)
  {
    switch (Serial.read())
    {
      case 'd':
        clearDumps();
        break;
    }
  };
} // loop()
