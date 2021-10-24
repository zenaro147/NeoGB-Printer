# NeoGB Printer

The NeoGB Printer is intended to be a standalone Game Boy Printer emulator very easy to build with parts readily available on the market. It does require little to no soldering skill and is very easy to flash. Using it is straigthforward and intuitive. Mounting the device with all parts on a table requires about a rainy afternoon from opening the bags to printing.

This project is very similar to a popular ready-to-use solution available on the market, but the NeoGB Printer is open-source, cheap and and tested with success with [all officially released games (110 in total)](https://docs.google.com/spreadsheets/d/1RQeTHemyEQnWHbKEhUy16cPxR6vA3YfeBbyx2tIXWaU/edit#gid=0) that support the [original Gameboy Printer](https://en.wikipedia.org/wiki/Game_Boy_Printer). The total cost for all the parts is about $10.

You just need to upload the code using the Arduino IDE, connect the components like described here, plug your SD card and print any image directly from a Gameboy compatible game. Serial protocol is directly recorded under binary form on SD card. Once your printing session is finished, hold the [button](#push-button-setup) (see below) for a few seconds and all the recorded data will be quickly converted to BMP and PNG images in separated folders, ready to use with social media.

## Software Setup
First of all, rename the `config.h.txt` to just `config.h` to import the pinout settings. This file contains all the options that can be changed in the emulator (pinout, OLED and LEDs features for example). Customize it according to the board and modules you are using.

To install the ESP32 board for the Arduino IDE, follow the [instructions here](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html). I highly recommended following the instructions to install via Boards Manager.

## Hardware Setup
This code has been created for a "DOIT ESP32 DEVKIT V1" [ESP32 based board](https://github.com/espressif/arduino-esp32/). All my tests was executed using [this chinese board](https://a.aliexpress.com/_mOCHLMT). You can use any other board available in the market. Just make sure of few points:\
* The number of total pins available in the board doesn't matter (Could be with 30 pins, 36 pins, 38 pins, whatever)
* It's a **Dual Core module** (some ESP32 modules are single core, like the ESP32-S2 and ESP32-C3)
* Have, at least, **2 SPI pins gourps** [like this example](https://4.bp.blogspot.com/-nGLtB2nUrDg/Wp6DQbzcJMI/AAAAAAAABq0/A6Z46p0SQSEdERWocWL94oUmeATMQre4wCLcBGAs/s1600/3.png) (normally it's called HSPI and VSPI, or sometimes have one called SPI and the other called VSPI or HSPI), make sure to check the pinout before buy one;

Some boards already have a SD Card slot built in. I've never tested this kind of board, but IN THEORY it should work fine (as long as it has SPI pins available). If you want to use this type of board, **DO IT AT YOUR OWN RISK!**

## Gameboy Link Cable Setup
Gameboy Original/Color Link Cable Pinout. If you don't want to sacrifice a Link Cable, you can use this [Gameboy Link Cable Breakout PCB](https://github.com/Palmr/gb-link-cable) to connect the pins and keep your Link Cable safely ! [You can buy this board here, from OSH Park](https://oshpark.com/shared_projects/srSgm3Yj). A bit of do-it-yourself is necessary at this step if you do not use a breakout board.

Different from Arduino that operates in 5V like the GameBoy, the ESP32 operate is in 3.3V on its pins by default. You will need a [tiny bidirectionnal Level Shifter like this](https://pt.aliexpress.com/item/1005001839292815.html) to handle the communication protocol and prevent any overvoltage/undervoltage from any side. Direct connection between Game Boy and ESP pins without level shifter may work but we do not recommand this for reliability reasons.

Connect the Game Boy serial pins to the ESP pins following this scheme:
```
 __________
|  6  4  2 |
 \_5__3__1_/ (at cable)

| Link Cable |Level Shifter|  ESP32  |
|------------|-------------|---------|
| Pin 1      |             |  N/A    | <- 5v from Game Boy (unreliable voltage source)
| Pin 2      |  HV1<->LV1  |  G19    | <- Serial Out (Serial In at the other end of the cable)
| Pin 3      |  HV2<->LV2  |  G23    | <- Serial In (Serial Out at the other end of the cable)
| Pin 4      |             |  N/A    | <- Serial Data
| Pin 5      |  HV3<->LV3  |  G18    | <- Clock Out
| Pin 6      |  GND<->GND  |  GND    | <- GND
|            |      LV     |  +3.3V  | <- +3.3 volts from ESP32
|            |      HV     |  +5V    | <- +5 volts from ESP32 or USB
```

## SD Card Reader Setup
You need to use a [Micro SD Card Module](https://pt.aliexpress.com/item/4000002592780.html) or a [SD Card Module](https://pt.aliexpress.com/item/32523666863.html) to save the data. I highly recommend to get one, especially the [SD Card Module](https://pt.aliexpress.com/item/32523666863.html), It's more stable than [Micro SD Card Module](https://pt.aliexpress.com/item/4000002592780.html), at least during my tests.
To use it, connect the pins following this scheme:
```
| SD ADAPTER |  ESP32  |
|------------|---------|
| CS         | G15     | 
| SCK        | G14     |
| MISO       | G27     | <-- DON'T USE THE G12... YOU CAN USE ANY OTHER PIN AVAILABLE
| MOSI       | G13     |
| GND        | G       |
| VCC/5v     | VIN     |
| 3v3        |   N/A   |

```

## Push Button Setup
You will need to add a [little Push Button like this](https://pt.aliexpress.com/item/1005002824489337.html) to convert all RAW data to BMP and PNG. To use it, just set the `#define BTN_PUSH` in `config.h` to any pin you want.

The function is simple:
* Single press: Force to refresh the image ID (only two games require this action: `E.T.: Digital Companion` and `Mary-Kate and Ashley Pocket Planner`)
* Long Press: Convert all RAW data to BMP

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

## RGB LED to display the Status (optional)
You can add a [simple RGB LED like this](https://pt.aliexpress.com/item/1005002535018824.html). This LED will be very useful to display the printer status, like Idle, Converting, Receiving Data, etc.

To use it, you need to uncomment `#define COMMON_ANODE` or `#define COMMON_CATHODE`, based on your LED. Edit the other legs in the `config.h` based on your setup.
 If your LED uses a Common Anode, connect it to the 3.3v Pin. If it's Common Cathode, connect it to the GND.
For the other legs, you need to use at least a 22O Ohm Resistor on each RGB leg. Connect them following the example scheme below (always based on your `config.h` file)
```
| LED | ESP32 |
|-----|-------|
|  R  |  16   | <-- YOU CAN USE ANY GPIO AVAILABLE. Connect at least a 220 Ohm Resistor with it.
|  G  |   4   | <-- YOU CAN USE ANY GPIO AVAILABLE. Connect at least a 220 Ohm Resistor with it.
|  B  |  17   | <-- YOU CAN USE ANY GPIO AVAILABLE. Connect at least a 220 Ohm Resistor with it.

```

## OLED Display Setup (optional)
You can add a [tiny oled display like this](https://pt.aliexpress.com/item/32672229793.html). To use it, you need to uncomment `#define USE_OLED` and the following lines. The display will show the current status of the printer.

```
| OLED DISPLAY |   ESP32   |
|--------------|-----------|
| GND          | GND       | 
| VIN          | 5v or 3v3 |
| SCL          | G22       | <-- YOU CAN USE ANY GPIO AVAILABLE
| SDA          | G21       | <-- YOU CAN USE ANY GPIO AVAILABLE

```

## Powering the beast
Any 5 Volts source available will do the job as the device consumes less than 1 W: powerbank with USB cable, Lithium battery with charger circuit, regular AA batteries with 5 volts regulator like the DD1205UA.

# Builds Showcases:
![My personal prototype build - zenaro147](/showcase/zenaro147.jpg)
![Setup by Raphaël BOICHOTy](/showcase/RaphaelBOICHOT.jpg)

## ⚠ Take care ⚠
You should not power the ESP from the GameBoy, as this might damage the GameBoy itself.

# Posts about:
* [Hack a Day Article](https://hackaday.com/2021/10/22/an-open-source-game-boy-printer-that-doesnt-print/)
* [Reddit post](https://www.reddit.com/r/Gameboy/comments/qdfoys/the_first_opensource_and_fully_standalone_game/)

# Based on existing projects:
* [Mofosyne - Arduino Gameboy Printer Emulator](https://github.com/mofosyne/arduino-gameboy-printer-emulator)
* [HerrZatacke - WiFi Game Boy Printer Emulator](https://github.com/HerrZatacke/wifi-gbp-emulator)
* [Raphaël BOICHOT - Gameboy Printer Paper Simulation](https://github.com/Raphael-Boichot/GameboyPrinterPaperSimulation)

# Authors contribution:
* Rafael Zenaro: main code, hardware setting, ideas, concept art and technical innovations.
* Brian Khuu: architect of the Matrix (Game Boy Printer emulator core, BMP image decoder core).
* Raphaël Boichot: debugging, monkey coding for PNG conversion and weirdos protocol support.

# ToDo List:
- [x] Update the code to support all games
- [x] Use the original BMP library from Mofosyne C decompiler
- [X] Write the BMP file on SD
- [X] Fix Palette issue with some games
- [X] Handle with storage % instead number of files
- [X] Parse the Output directory too, to return the next image ID
- [X] Update documetation (wiring, compatible devices, etc)
- [X] Improve the LED status using a RGB LED
- [ ] Add support to PNG in addition to BMP
- [ ] Add a message to display the number of images printed

# List games fully compatible with the NeoGB Printer:
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

