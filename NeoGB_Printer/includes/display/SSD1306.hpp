#define LGFX_USE_V1       // v1.0.0 を有効にします(v0からの移行期間の特別措置とのこと。書かない場合は旧v0系で動作)
#include <LovyanGFX.hpp>  // lovyanGFXのヘッダを準備

// 独自の設定を行うクラスを、LGFX_Deviceから派生して作成します。
// クラス名はLGFXからLGFX_SSD1306に変更してます。（コンストラクタ名も）
class LGFX_SSD1306 : public lgfx::LGFX_Device {
  // 接続するOLED表示器にあったインスタンスを準備します。
  lgfx::Panel_SSD1306   _panel_instance;  // SSD1306を使用する場合
  lgfx::Bus_I2C _bus_instance;          // I2Cバスのインスタンス (ESP32のみ)

  // コンストラクタを作成し、ここで各種設定を行います。
  public:
    LGFX_SSD1306(void) {  // コンストラクタ名はクラス名に合わせてLGFXからLGFX_SSD1306に変更してます。（クラス名と同じにする）
      { // バス制御の設定を行います。
        auto cfg = _bus_instance.config();    // バス設定用の構造体を取得します。
        // I2Cバスの設定
        cfg.i2c_port    = 0;          // 使用するI2Cポートを選択 (0 or 1)
        cfg.freq_write  = 400000;     // 送信時のクロック
        cfg.freq_read   = 400000;     // 受信時のクロック
        cfg.pin_sda     = OLED_SDA;         // SDAを接続しているピン番号
        cfg.pin_scl     = OLED_SCL;         // SCLを接続しているピン番号
        cfg.i2c_addr    = 0x3C;       // I2Cデバイスのアドレス

        _bus_instance.config(cfg);    // 設定値をバスに反映します。
        _panel_instance.setBus(&_bus_instance);      // バスをパネルにセットします。
      }

      { // 表示パネル制御の設定を行います。
        auto cfg = _panel_instance.config();    // 表示パネル設定用の構造体を取得します。

        cfg.pin_cs           =    -1;  // CSが接続されているピン番号   (-1 = disable)
        cfg.pin_rst          =    -1;  // RSTが接続されているピン番号  (-1 = disable)
        cfg.pin_busy         =    -1;  // BUSYが接続されているピン番号 (-1 = disable)

        // ※ 以下の設定値はパネル毎に一般的な初期値が設定されていますので、不明な項目はコメントアウトして試してみてください。

        cfg.panel_width      =   SCREEN_WIDTH;  // 実際に表示可能な幅
        cfg.panel_height     =   SCREEN_HEIGHT;  // 実際に表示可能な高さ
        cfg.offset_x         =     0;  // パネルのX方向オフセット量
        cfg.offset_y         =     0;  // パネルのY方向オフセット量		
        cfg.offset_rotation  =     2;  // 回転方向の値のオフセット 0~7 (4~7は上下反転)
//      cfg.dummy_read_pixel =     8;  // ピクセル読出し前のダミーリードのビット数
//      cfg.dummy_read_bits  =     1;  // ピクセル以外のデータ読出し前のダミーリードのビット数
//      cfg.readable         =  false;  // データ読出しが可能な場合 trueに設定
        cfg.invert           = false;  // パネルの明暗が反転してしまう場合 trueに設定
//      cfg.rgb_order        = false;  // パネルの赤と青が入れ替わってしまう場合 trueに設定
//      cfg.dlen_16bit       = false;  // 16bitパラレルやSPIでデータ長を16bit単位で送信するパネルの場合 trueに設定
//      cfg.bus_shared       =  true;  // SDカードとバスを共有している場合 trueに設定(drawJpgFile等でバス制御を行います)

// 以下はST7735やILI9163のようにピクセル数が可変のドライバで表示がずれる場合にのみ設定してください。
//      cfg.memory_width     =   SCREEN_WIDTH;  // ドライバICがサポートしている最大の幅
//      cfg.memory_height    =   SCREEN_HEIGHT;  // ドライバICがサポートしている最大の高さ

        _panel_instance.config(cfg);
        switch (OLED_COMPINS) {
          case 1:            
            _panel_instance.setComPins(0x02);
            break;
          case 2:
            _panel_instance.setComPins(0x12);
            break;
          case 3:
            _panel_instance.setComPins(0x22);
            break;
          case 4:
            _panel_instance.setComPins(0x32);
            break;
          default:
            _panel_instance.setComPins(0x02);
        }

      }
      setPanel(&_panel_instance);     // 使用するパネルをセットします。
    }
};