# The NeoGB Printer - ポケットプリンタエミュレータ

The NeoGB Printer is a SD card-based standalone Game Boy Printer emulator. It is easy to build with parts readily available on the market. It does require little to no soldering skill and is fast to install from Arduino IDE (few dependancies). Using it is straigthforward and intuitive. Mounting the device with all parts on a table requires about an afternoon from opening the component bags to printing.

This project is very similar to a popular ready-to-use solution available on the market, but the NeoGB Printer is open-source, cheap and tested with success with [all officially released games (110 in total)](https://docs.google.com/spreadsheets/d/1RQeTHemyEQnWHbKEhUy16cPxR6vA3YfeBbyx2tIXWaU/edit#gid=0) that support the [original Gameboy Printer](https://en.wikipedia.org/wiki/Game_Boy_Printer). The total cost for all the parts bougth new is below $15. All parts can be easily exchanged with other projects, reused or harvested from dead electronics as they are all very common.

You just need to upload the code using the Arduino IDE, connect the components like described here, plug your SD card and print any image directly from a Gameboy compatible game. Serial protocol is directly recorded under binary form on SD card. Once your printing session is finished, hold the [button](#push-button-setup) (see below) for a few seconds and all the recorded data will be quickly converted to BMP and/or PNG images, ready to use with social media. Reboot the device once and you will access to an intuitive webserver mode.

## Software Setup
First of all, rename the `config.h.txt` to just `config.h` to import the pinout settings. This file contains all the options that can be changed in the emulator (pinout, OLED and LEDs features, webserver parameters, scaling factor for images, etc.). Customize it according to the board and modules you are using.

To install the ESP32 board for the Arduino IDE, follow the [instructions here](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html). I highly recommended following the instructions to install via Boards Manager.

If you want to use the OLED Display, you need to install two Libraries from the Arduino Library Manager - `Tools > Manage Libraries...`
* Adafruit SSD1306
* Adafruit GFX Library

## SD card Setup
Format any size SD card in FAT32 and copy the content of the 'SD' folder to the SD card, so that **./www/** and **./E_paper/** directories are at the root. 
* **./www** contains the files for the webserver.
* **./E_paper/** contains a GNU Octave/Matlab script to transform your printer images into printings simulating the output of an actual Game Boy Printer (aka [e-paper](https://github.com/Raphael-Boichot/GameboyPrinterPaperSimulation)). 

During use of the NeoGB Printer, new files will appear on the card. 
* **./ID_storage.bin** generates an unique ID for each print so avoid to delete it (**if deleted, the image count will restart from 1 with a risk of overwriting existing images)**. 
* Your images will be placed at **./output/bmp/** and **./output/png/** and can be deleted as you wish.

## Hardware Setup
This code has been created for a "DOIT ESP32 DEVKIT V1" [ESP32 based board](https://github.com/espressif/arduino-esp32/). All my tests was executed using [this chinese board](https://a.aliexpress.com/_mOCHLMT). You can use any other board available in the market. Just make sure of few points:
* The number of total pins available in the board doesn't matter (Could be with 30 pins, 36 pins, 38 pins, whatever)
* It's a **Dual Core module** (some ESP32 modules are single core, like the ESP32-S2 and ESP32-C3)
* Have, at least, **2 SPI pins groups** [like this example](https://4.bp.blogspot.com/-nGLtB2nUrDg/Wp6DQbzcJMI/AAAAAAAABq0/A6Z46p0SQSEdERWocWL94oUmeATMQre4wCLcBGAs/s1600/3.png) (normally it's called HSPI and VSPI, or sometimes have one called SPI and the other called VSPI or HSPI), make sure to check the pinout before buy one;

Some boards already have a SD Card slot built in. We've never tested this kind of board, but *in theory* it should work fine (as long as it has SPI pins available). If you choose to use this type of board, **we however won't be able to provide any support in case of bug**...But feel free to experiment.

## Gameboy Link Cable Setup
Gameboy Original/Color Link Cable Pinout. If you don't want to sacrifice a Link Cable, you can use this [Gameboy Link Cable Breakout PCB](https://github.com/Palmr/gb-link-cable) to connect the pins and keep your Link Cable safely ! [You can buy this board here, from OSH Park](https://oshpark.com/shared_projects/srSgm3Yj). A bit of do-it-yourself is necessary at this step if you do not use a breakout board.

Different from Arduino that operates in 5V like the GameBoy, the ESP32 operate is in 3.3V on its pins by default. You will need a [tiny bidirectionnal Level Shifter like this](https://pt.aliexpress.com/item/1005001839292815.html) to handle the communication protocol and prevent any overvoltage/undervoltage from any side. Direct connection between Game Boy and ESP pins without level shifter may work but we do not recommend this for long term reliability reasons.

Connect the Game Boy serial pins to the ESP pins following this scheme:
```
 __________
|  6  4  2 |
 \_5__3__1_/ (at cable, front view, Game boy side)

| Link Cable |Level Shifter|  ESP32  |
|------------|-------------|---------|
| Pin 1      |             |   N/A   | <- 5v from Game Boy (unnecessary)
| Pin 2      |  HV1<->LV1  |   G19   | <- Serial Out (Game Boy side)
| Pin 3      |  HV2<->LV2  |   G23   | <- Serial In (Game Boy side)
| Pin 4      |             |   N/A   | <- Serial Data (unnecessary)
| Pin 5      |  HV3<->LV3  |   G18   | <- Clock Out
| Pin 6      |  GND<->GND  |   GND   | <- GND
|            |      LV     |  +3.3V  | <- +3.3 volts from ESP32
|            |      HV     |   +5V   | <- +5 volts from ESP32 or USB
```
**⚠If the connection does not work, uncomment the line `#define INVERT_SERIAL_PINS` in config.h !⚠**

## SD Card Reader Setup
You need to use a [Micro SD Card Module](https://pt.aliexpress.com/item/4000002592780.html) or a [SD Card Module](https://pt.aliexpress.com/item/32523666863.html) to save the data. I highly recommend to get one, especially the [SD Card Module](https://pt.aliexpress.com/item/32523666863.html), It's more stable than [Micro SD Card Module](https://pt.aliexpress.com/item/4000002592780.html), at least during my tests.
To use it, connect the pins following this scheme:
```
| SD ADAPTER |  ESP32  |
|------------|---------|
| CS         |   G15   | 
| SCK        |   G14   |
| MISO       |   G26   | <-- DON'T USE THE G12! YOU CAN USE ANY OTHER PIN AVAILABLE
| MOSI       |   G13   |
| GND        |   GND   |
| VCC/5v     |   VIN   |
| 3v3        |   N/A   |

```

## Push Button Setup
You will need to add a [little Push Button like this](https://pt.aliexpress.com/item/1005002824489337.html) to convert all RAW data to BMP and/or PNG. To use it, just set the `#define BTN_PUSH` in `config.h` to any pin you want.

The function is simple:
* Single short press: Force to refresh the image ID after printing (only two games require this action: `E.T.: Digital Companion` and `Mary-Kate and Ashley Pocket Planner`).
* Long Press: Convert all RAW data to BMP/PNG images.

```
PushButton Schematic
     __________
    |          |
1 --|----------|-- 2
    |          |
3 --|----------|-- 4
    |__________|

| Button | ESP32 |
|--------|-------|
| 1 or 2 |  3v3  | 
| 3 or 4 |  G34  | <-- I recommend to connect a 5K to 10K resistor to the GND together, to act as a Pull Down.

```

## LED to display the Status (optional)
You can add a [simple RGB LED like this](https://pt.aliexpress.com/item/1005002535018824.html) and/or a [standard one](https://pt.aliexpress.com/item/1936218827.html). This LED will be very useful to display the printer status, like Idle, Converting, Receiving Data, etc.

To use it, you need to uncomment `#define COMMON_ANODE` or `#define COMMON_CATHODE` for RGB LED (based on your LED) or `#define LED_STATUS_PIN` to use the standard one. Edit the pins in the `config.h` based on your setup.
If your RGB LED uses a Common Anode, connect it to the 3.3v Pin. If it's Common Cathode, connect it to the GND.
For the other legs, you need to use at least a 220 Ohm Resistor on each RGB leg. Connect them following the example scheme below (always based on your `config.h` file)
```
| RGB | ESP32 |
|-----|-------|
|  R  |  G16  | <-- YOU CAN USE ANY GPIO AVAILABLE. Connect at least a 220 Ohm Resistor with it.
|  G  |  G17  | <-- YOU CAN USE ANY GPIO AVAILABLE. Connect at least a 220 Ohm Resistor with it.
|  B  |  G4   | <-- YOU CAN USE ANY GPIO AVAILABLE. Connect at least a 220 Ohm Resistor with it.

```
If you are using a standard single color LED, connect it by following the example scheme below (always based on your `config.h` file). Using a single color LED in combination with OLED display is an interesting setup.
```
| LED | ESP32 |
|-----|-------|
|  +  |  G5   | <-- ANODE LED (the long one) - YOU CAN USE ANY GPIO AVAILABLE. Connect at least a 220 Ohm Resistor with it.
|  -  |  GND  | <-- CATHODE LED (the short one)
```

## OLED Display Setup (optional)
You can add a [tiny oled display like this](https://pt.aliexpress.com/item/32672229793.html). To use it, you need to uncomment `#define USE_OLED` and the following lines. The display will show the current status of the printer.

```
| OLED DISPLAY |   ESP32   |
|--------------|-----------|
| GND          |    GND    | 
| VIN          | 5v or 3v3 |
| SCL          |    G22    | <-- YOU CAN USE ANY GPIO AVAILABLE
| SDA          |    G21    | <-- YOU CAN USE ANY GPIO AVAILABLE

```

## WebServer interface (optional)
The NeoGB Printer has an integrated WebServer to easily download your photos and delete them too. The printer will alternate between `Printer Mode` (to print and generate the image files) and `Server Mode` (to access the Web interface and manage your pictures) on each boot.

If you want to use it, you can uncomment the line `#define ENABLE_WEBSERVER` from the `config.h`. Right below, you can edit some information about it too. In case the program can't connect to an existent WiFi network, the NeoGB Printer will automatically enters in Hotspot mode and will host a WiFi network called `gameboyprinter`
- **DEFAULT_MDNS_NAME** : MDNS Hostname (useful for Apple products to connect instead the IP address) 
- **DEFAULT_AP_SSID** : Your WiFi network SSID
- **DEFAULT_AP_PSK** : Yout WiFi network password
- **WIFI_CONNECT_TIMEOUT** : Wifi timeout connection in ms

If you are using the OLED display, the IP address will shows on it to easily connect to you printer.

## Powering the beast
Any 5 Volts source available will do the job as the device consumes less than 1 W: powerbank with USB cable, mobile phone with OTG cable, lithium battery with charger circuit, regular AA batteries with 5 volts regulator like the DD1205UA, etc.

## How to use it ? Printer mode
At each reboot, the device will alternate between Printer mode and Webserver mode.
* Power the ESP, white LED flashes 3 times immediately, followed by an RGB test (Must blink Red, Green and Blue in this exact order. Otherwise, change the pin order in the `config.h`).  Wait for the next 3 green flashes indicating that the filesystem is ready for printing.
* Print as with the real Game Boy Printer, as many prints as you wish in a single session. Data are stored in binaries .bin files called "dumps" on the SD card. Batch printing with Game Boy Camera is of course possible.
* Some rare games require a short press on pushbutton to separate the files after printing as they do not have a margin indication in the print command. If button is pressed short, magenta led flashes one second to indicate that command have been acknowledged.
* In the same session or later after a reboot, press the pushbutton for about 2 seconds to convert all .bin binaries in .bmp, .png or both. The scaling factor could be independently chosen between 1 and any value for each output format. The conversion begins and ends with 3 blue flashes. Each image requires some time to be converted (depending on the scaling factor used), so convert them regularly and/or be patient.
* Remove the SD card, connect to a computer and enjoy your upscaled images ready to be published online !
* You can at this step run an [e-paper simulator](https://github.com/Raphael-Boichot/GameboyPrinterPaperSimulation) with a GNU Octave/Matlab interpreter by running **NeoGBPrinter_E_paper.m** directly from the SD card.
* There is an easter egg in the printer, will you find it ?

## How to use it ? Webserver mode
At each reboot, the device will alternate between Printer mode and Webserver mode.
* Power the ESP, white LED flashes 3 times immediately, followed by an RGB test (Must blink Red first, than Green and than Blue. In this exactly order. Otherwise, change the pin order in the `config.h`), wait for the next 3 green flashes indicating that the filesystem is ready for Webserver mode. **Booting time may take some time if you have a huge number of images on the SD card as webserver build the file list during this step.**
* Access directly your images at http://gameboyprinter. The OLED screen indicate the IP address of the site too.

OR

* Access your images at https://herrzatacke.github.io/gb-printer-web/#/
- Settings->Printer URL (enter the IP given on the OLED or the Arduino IDE serial)
- Import->Open Printer page->Check Printer->Fetch Images
- Gallery. Now you can play with palettes and export your images.

## User manual in brief
![user_manual](/Supplementary_images/User_manual.png)

# Builds Showcases:
![My personal prototype build - zenaro147](/showcase/zenaro147.jpg)
![Setup by Raphaël BOICHOTy](/showcase/RaphaelBOICHOT.jpg)

## ⚠ Take care ⚠
You should not power the ESP from the GameBoy, as this might damage the GameBoy itself. The +5 volts from Game Boy serial is not a reliable power source.

# Posts about:
* [Hack a Day Article](https://hackaday.com/2021/10/22/an-open-source-game-boy-printer-that-doesnt-print/)
* [Reddit post](https://www.reddit.com/r/Gameboy/comments/qdfoys/the_first_opensource_and_fully_standalone_game/)

# Based on existing projects:
* [Mofosyne - Arduino Gameboy Printer Emulator](https://github.com/mofosyne/arduino-gameboy-printer-emulator)
* [HerrZatacke - WiFi Game Boy Printer Emulator](https://github.com/HerrZatacke/wifi-gbp-emulator)
* [Raphaël BOICHOT - Gameboy Printer Paper Simulation](https://github.com/Raphael-Boichot/GameboyPrinterPaperSimulation)

# Authors contribution:
* Rafael Zenaro: main code, hardware setting, new ideas, concept art, technical innovations and group facilitation.
* Brian Khuu: architect of the Matrix, Game Boy Printer emulator core, BMP image decoder core.
* Raphaël Boichot: serial protocol and code debugging, BMP and PNG upscalers, RGB led support, hardcore gaming with Japanese kusoge.
* Cristofer Cruz: 3D model for the GB Printer shell.
* Herr_Zatacke: support for the Game Boy Printer Web compatibility.

Want to discuss with the authors or share your art and projects with people fond of the Game Boy Camera and Pocket Printer ? **Join the Game Boy Camera Club Discord**: https://discord.gg/dKND7cFuqM

![credits](/Supplementary_images/credits.png)

# List of projects compatible with the NeoGB Printer:
* [reini1305 - gb_bannerprinter](https://github.com/reini1305/gb_bannerprinter) - perfect emulation.
* [The-Arduino-SD-Game-Boy-Printer](https://github.com/Raphael-Boichot/The-Arduino-SD-Game-Boy-Printer) - it is convoluted but possible. It can be used as debug tool.
* [Blarble1290](https://8bittygames.com/blarble1290/) - it works if you convert the dumps between each dialog round, but you have to reset manually the dump counter at the end of a playing session into ID_storage.bin (last byte must be 0x00) and erase the remaining dumps into the ./dumps folder. The Games itself prints with actual Game Boy Printers, not with actual Pocket Printers which probably implies timing issues within the protocol used.

# List of games tested and fully compatible with the NeoGB Printer:
- *1942 (never released in Japan)*
- *Alice in Wonderland (never released in Japan)*
- *Animal Breeder 3 (あにまるぶりーだー3)*
- *Animal Breeder 4 (あにまるぶり〜だ〜4)*
- *Aqualife (アクアライフ)*
- *Asteroids (never released in Japan)*
- *Austin Powers: Oh, Behave! (never released in Japan)*
- *Austin Powers: Welcome to My Underground Lair! (never released in Japan)*
- *Austin Power Episode 3 - Yeah Baby Yeah (unreleased, rom CGBBA3P0.3)*
- *Austin Power Episode 4 - Why Make Millions (unreleased, rom CGBBA4P0.0)*
- *Cardcaptor Sakura: Itsumo Sakura-chan to Issho! (カードキャプターさくら 〜いつもさくらちゃんと一緒〜)*
- *Cardcaptor Sakura: Tomoe Shōgakkō Daiundōkai (カードキャプターさくら 〜友枝小学校大運動会〜)*
- *Chee-Chai Alien (ちっちゃいエイリアン)*
- *Cross Hunter - Monster Hunter Version (クロスハンター モンスター・ハンター・バージョン)*
- *Cross Hunter - Treasure Hunter (クロスハンター トレジャー・ハンター・バージョン)*
- *Cross Hunter - X Hunter Version (クロスハンター エックス・ハンター・バージョン)*
- *Daa! Daa! Daa! Totsuzen ★ Card de Battle de Uranai de!? (だぁ!だぁ!だぁ! とつぜん★カードでバトルで占いで!?)*
- *Daikaijuu Monogatari: The Miracle of the Zone II (大貝獣物語 ザ・ミラクル オブ ザ・ゾーンII)*
- *Dejiko no Mahjong Party (でじこの麻雀パーティー)*
- *Densha de GO! 2 (電車でGO!2)*
- *Dino Breeder 3 - Gaia Fukkatsu (ディノブリーダー3 〜ガイア復活〜)*
- *Disney's Dinosaur (never released in Japan)*
- *Disney's Tarzan (ディズニーズ ターザン)*
- *Donkey Kong Country (ドンキーコング2001)*
- *Doraemon Kart 2 (ドラえもんカート2)*
- *Doraemon Memories - Nobita no Omoide Daibouken (ドラえもんメモリーズ のび太の想い出大冒険)*
- *Doraemon no Game Boy de Asobouyo Deluxe 10 (ドラえもんのGBであそぼうよ デラックス10)*
- *Doraemon no Quiz Boy (ドラえもんのクイズボーイ)*
- *Dungeon Savior (ダンジョンセイバー)*
- *E.T.: Digital Companion (never released in Japan)*
- *Fairy Kitty no Kaiun Jiten: Yousei no Kuni no Uranai Shugyou (フェアリーキティの開運辞典 妖精の国の占い修行)*
- *Fisher-Price Rescue Heroes: Fire Frenzy (never released in Japan)*
- *Game Boy Camera or Pocket Camera (ポケットカメラ)*
- *Golf Ou: The King of Golf (ゴルフ王)*
- *Hamster Club (ハムスター倶楽部)*
- *Hamster Paradise (ハムスターパラダイス)*
- *Hamster Paradise 2 (ハムスターパラダイス2)*
- *Harvest Moon 2 (牧場物語GB2)*
- *Hello Kitty no Beads Koubou (ハローキティのビーズ工房)*
- *Hello Kitty no Magical Museum (ハローキティのマジカルミュージアム)*
- *Hello Kitty Pocket Camera (ハローキティのポケットカメラ, unreleased, rom GBDHKAJ0.2)*
- *Jinsei Game Tomodachi takusan Tsukurou Yo! (人生ゲーム 友達たくさんつくろうよ!)*
- *Kakurenbo Battle Monster Tactics (モンスタータクティクス)*
- *Kanji Boy (漢字BOY)*
- *Karamuchou wa Oosawagi!: Porinkiis to Okashina Nakamatachi (カラムー町は大さわぎ! 〜ポリンキーズとおかしな仲間たち〜)*
- *Karamuchou wa Oosawagi!: Okawari! (カラムー町は大さわぎ！おかわりっ！)*
- *Kaseki Sousei Reborn II: Monster Digger (化石創世リボーン2 〜モンスターティガー〜)*
- *Kettou Transformers Beast Wars - Beast Senshi Saikyou Ketteisen (決闘トランスフォーマービーストウォーズ ビースト戦士最強決定戦)*
- *Kidou Senkan Nadesico - Ruri Ruri Mahjong (機動戦艦ナデシコ ルリルリ麻雀)*
- *Kisekae Monogatari (きせかえ物語)*
- *Klax (never released in Japan)*
- *Konchuu Hakase 2 (昆虫博士2)*
- *Little Nicky (never released in Japan)*
- *Logical (never released in Japan)*
- *Love Hina Pocket (ラブ ひな)*
- *Magical Drop (never released in Japan)*
- *Mary-Kate and Ashley Pocket Planner (never released in Japan)*
- *McDonald's Monogatari : Honobono Tenchou Ikusei Game (マクドナルド物語)*
- *Mickey's Racing Adventure (never released in Japan)*
- *Mickey's Speedway USA (never released in Japan)*
- *Mission: Impossible (never released in Japan)*
- *Monster ★ Race 2 (もんすたあ★レース2)*
- *Monster ★ Race Okawari (もんすたあ★レース おかわり)*
- *Nakayoshi Cooking Series 1 - Oishii Cake-ya-san (なかよしクッキングシリーズ1 おいしいケーキ屋さん)*
- *Nakayoshi Cooking Series 2 - Oishii Panya-san (なかよしクッキングシリーズ2 おいしいパン屋さん)*
- *Nakayoshi Cooking Series 3 - Tanoshii Obentou (なかよしクッキングシリーズ3 たのしいお弁当)*
- *Nakayoshi Cooking Series 4 - Tanoshii Dessert (なかよしクッキングシリーズ4 たのしいデザート)*
- *Nakayoshi Cooking Series 5 - Cake Wo Tsukurou (なかよしクッキングシリーズ5 こむぎちゃんのケーキをつくろう!)*
- *Nakayoshi Pet Series 1: Kawaii Hamster (なかよしペットシリーズ1 かわいいハムスタ)*
- *Nakayoshi Pet Series 2: Kawaii Usagi (なかよしペットシリーズ2 かわいいウサギ)*
- *Nakayoshi Pet Series 3: Kawaii koinu (なかよしペットシリーズ3 かわいい仔犬)*
- *NFL Blitz (never released in Japan)*
- *Nintama Rantarou GB: Eawase Challenge Puzzle (忍たま乱太郎GB えあわせチャレンジパズル)*
- *Ojarumaru: Mitsunegai Jinja no Ennichi de Ojaru! (おじゃる丸 〜満願神社は縁日でおじゃる!)*
- *Pachinko Data Card - Chou Ataru-kun (Pachinko Data Card ちょ〜あたる君〜)*
- *Perfect Dark (never released in Japan)*
- *Pocket Family 2 (ポケットファミリーGB2)*
- *Pocket Kanjirou (ポケット漢字郎)*
- *Pocket Puyo Puyo-n (ぽけっとぷよぷよ〜ん)*
- *Pokémon Card GB2: Great Rocket-Dan Sanjō! (ポケモンカードGB2 GR団参上!)*
- *Pokémon Crystal (ポケットモンスター クリスタルバージョン)*
- *Pokémon Gold (ポケットモンスター 金)*
- *Pokémon Picross (ポケモンピクロス, unreleased, rom DMGAKVJ0.1)*
- *Pokémon Pinball (ポケモンピンボール)*
- *Pokémon Silver (ポケットモンスター 銀)*
- *Pokémon Trading Card Game (ポケモンカードGB)*
- *Pokémon Yellow: Special Pikachu Edition (ポケットモンスター ピカチュウ)*
- *Pro Mahjong Tsuwamono GB (プロ麻雀兵 GB)*
- *Purikura Pocket 3 - Talent Debut Daisakusen (プリクラポケット3 〜タレントデビュー大作戦〜)*
- *Puzzled (never released in Japan)*
- *Quest for Camelot (never released in Japan)*
- *Roadsters Trophy (never released in Japan)*
- *Sanrio Timenet: Kako Hen (サンリオタイムネット 過去編)*
- *Sanrio Timenet: Mirai Hen (サンリオタイムネット 未来編)*
- *Shinseiki Evangelion Mahjong Hokan Keikaku (新世紀エヴァンゲリオン 麻雀補完計画)*
- *SMARTCOM (never released in Japan)*
- *Sōko-ban Densetsu: Hikari to Yami no Kuni (倉庫番伝説 光と闇の国)*
- *Super Black Bass Pocket 3 (スーパーブラックバスポケット3)*
- *Super Mario Bros. Deluxe (スーパーマリオブラザーズデラックス)*
- *Sweet Angel (スウィートアンジェ)*
- *Sylvanian Families: Otogi no Kuni no Pendant (シルバニアファミリー 〜おとぎの国のペンダント〜)*
- *Sylvanian Families 2 - Irozuku Mori no Fantasy (シルバニアファミリー2～色づく森のファンタジー)*
- *Sylvanian Families 3 - Hoshi Furu Yoru no Sunadokei (シルバニアファミリー３　星ふる夜のすなどけい)*
- *Tales of Phantasia: Nakiri's Dungeon (テイルズ オブ ファンタジア なりきりダンジョン)*
- *The Legend of Zelda: Link's Awakening DX (ゼルダの伝説 夢をみる島DX)*
- *The Little Mermaid 2: Pinball Frenzy (never released in Japan)*
- *Tony Hawk's Pro Skater 2 (never released in Japan)*
- *Trade & Battle: Card Hero (トレード&バトル カードヒーロー)*
- *Tsuri Sensei 2 (釣り先生2)*
- *VS Lemmings (VS.レミングス)*

## Things that can be improved in the future (or not... but feel free to contribute)
* A better/responsive Web interface. Since the actual is pretty simple.
* RTC (Real Time Clock) can be implemented but is not planned with this device. RTC obliges the device to be constantly powered even if it can be placed easily in deep sleep mode. Consequence is that the image files generated on SD card do not own time attributes. Copying the files into your favorite storage folder fixes this "issue".
* The file conversion to PNG uses BMP image as source material. It would be perfectly possible to embed a faster PNG converter that would directly use the binary stream of data. It is not planned for today as png support is yet fast.
* The BMP upscaling is quite slow due to the lack of onboard memory and because it makes two things. First, all images are processed by reading/writing to the SD card on the fly, using a very limited buffer (one line of pixels per image at most). Second, it compresses the native 24-bits BMP format output by the C decoder library to 4-bits indexed BMP in order to save storage space.
* The ESP is forced to run at 80 MHz instead of its native 240 MHz during serial protocol. At full speed we experienced protocol unstability with few games. We did not investigate what was the reason (faulty boards sold on Aliexpress ? Need for a magic cap on serial line ?). Anyway, image conversion is made at full speed, so experience for the user is not altered by this programming design choice.
* The choice of implementing a short press button function for dealing with some games is due to the fact that automatic splitting of images can be tricky as Game Boy Printer protocol specifications appeared to be rather unstrict. On the 110 games, most of them sends an after margin after printing one image to feed the paper and allow proper cutting. But sometimes, games send an after margin within image for aesthetic reason or no margin at all between images (but white packets instead). This render the automatic splitting of images all but obvious. Reverse engineering the protocol on 110 games was a very huge effort and we have debated a lot on how dealing with particular games that are perhaps totally unknown or forgotten. We hope that the solution provided here is the most convenient for users. It is at least the most conservative from a video game history point of view.
