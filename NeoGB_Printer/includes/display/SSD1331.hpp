#define LGFX_USE_V1       // v1.0.0 を有効にします(v0からの移行期間の特別措置とのこと。書かない場合は旧v0系で動作)
#include <LovyanGFX.hpp>  // lovyanGFXのヘッダを準備

// 独自の設定を行うクラスを、LGFX_Deviceから派生して作成します。
// クラス名はLGFXからLGFX_SSD1306に変更してます。（コンストラクタ名も）
class LGFX_SSD1331 : public lgfx::LGFX_Device {
  // 接続するOLED表示器にあったインスタンスを準備します。
  lgfx::Panel_SSD1331   _panel_instance;  // SSD1306を使用する場合
  lgfx::Bus_SPI _bus_instance;          // I2Cバスのインスタンス (ESP32のみ)

  // コンストラクタを作成し、ここで各種設定を行います。
  public:
    LGFX_SSD1331() {  // コンストラクタ名はクラス名に合わせてLGFXからLGFX_SSD1306に変更してます。（クラス名と同じにする）
      { // バス制御の設定を行います。
        // Set the bus control.
        auto cfg = _bus_instance.config (); // Get the structure for bus configuration.
  
        // SPI bus settings
        cfg.spi_host = HSPI_HOST; // Select the SPI to use ESP32-S2, C3: SPI2_HOST or SPI3_HOST / ESP32: VSPI_HOST or HSPI_HOST
        // * With the ESP-IDF version upgrade, the description of VSPI_HOST and HSPI_HOST is deprecated, so if an error occurs, use SPI2_HOST, SPI3_HOST instead.
        cfg.spi_mode = 0; // Set SPI communication mode (0 ~ 3)
        cfg.freq_write = 40000000; // SPI clock at the time of transmission (up to 80MHz, rounded to 80MHz divided by an integer)
        cfg.freq_read = 16000000; // SPI clock on reception
        cfg.spi_3wire = true; // Set true when receiving with MOSI pin
        cfg.use_lock = true; // Set true if transaction lock is used
        cfg.dma_channel = SPI_DMA_CH_AUTO; // Set the DMA channel to use (0 = DMA not used / 1 = 1ch / 2 = ch / SPI_DMA_CH_AUTO = automatic setting)
        // * With the ESP-IDF version upgrade, SPI_DMA_CH_AUTO (automatic setting) is recommended for the DMA channel. Specifying 1ch and 2ch is deprecated.
        cfg.pin_sclk = OLED_SCLK; // Set the SPI SCLK pin number
        cfg.pin_mosi = OLED_MOSI; // Set the SPI MOSI pin number
        cfg.pin_miso = OLED_MISO; // Set the SPI MISO pin number (-1 = disable)
        cfg.pin_dc = OLED_DC; // Set SPI D / C pin number (-1 = disable)
  
        _bus_instance.config (cfg); // Reflect the setting value on the bus.
        _panel_instance.setBus (& _bus_instance); // Set the bus to the panel.
      }
      { // 表示パネル制御の設定を行います。
        auto cfg = _panel_instance.config (); // Get the structure for display panel settings.
  
        cfg.pin_cs = OLED_CS; // Pin number to which CS is connected (-1 = disable)
        cfg.pin_rst = OLED_RST; // Pin number to which RST is connected (-1 = disable)
        cfg.pin_busy = -1; // Pin number to which BUSY is connected (-1 = disable)
  
        cfg.memory_width = SCREEN_WIDTH; // Maximum width supported by driver IC
        cfg.memory_height = SCREEN_HEIGHT; // Maximum height supported by driver IC
        //cfg.panel_width = 96; // Actually displayable width
        //cfg.panel_height = 64; // Height that can actually be displayed
        cfg.offset_x = 0; // Amount of offset in the X direction of the panel
        cfg.offset_y = 0; // Amount of offset in the Y direction of the panel
        cfg.offset_rotation = 2; // Offset of the value in the rotation direction 0 ~ 7 (4 ~ 7 is upside down)
        #ifdef OLED_ROTATE
        cfg.offset_rotation  =     2;  // Offset of the value in the rotation direction 0 ~ 7 (4 ~ 7 is upside down)
        #else
        cfg.offset_rotation  =     0;  // Offset of the value in the rotation direction 0 ~ 7 (4 ~ 7 is upside down)
        #endif
        //cfg.dummy_read_pixel = 8; // Number of dummy read bits before pixel read
        //cfg.dummy_read_bits = 1; // Number of bits of dummy read before reading data other than pixels
        cfg.readable = false; // Set to true if data can be read
        //cfg.invert = false; // Set to true if the light and darkness of the panel is reversed
        //cfg.rgb_order = false; // Set to true if the red and blue of the panel are swapped
        //cfg.dlen_16bit = false; // Set to true for panels that send data length in 16-bit units
        //cfg.bus_shared = true; // If the bus is shared with the SD card, set to true (bus control is performed with drawJpgFile etc.)
  
        _panel_instance.config (cfg);
      }
      setPanel(&_panel_instance);     // 使用するパネルをセットします。
    }
};