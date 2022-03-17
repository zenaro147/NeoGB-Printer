# The NeoGB Printer - ポケットプリンタエミュレータ

The NeoGB Printer is a SD card-based standalone Game Boy Printer emulator. It is easy to build with parts readily available on the market. It does require little to no soldering skill and is fast to install from Arduino IDE (few dependancies). Using it is straigthforward and intuitive. Mounting the device with all parts on a table requires about an afternoon from opening the component bags to printing.

This project is very similar to a popular ready-to-use solution available on the market, but the NeoGB Printer is open-source, cheap and tested with success with [all officially released games (110 in total)](https://docs.google.com/spreadsheets/d/1RQeTHemyEQnWHbKEhUy16cPxR6vA3YfeBbyx2tIXWaU/edit#gid=0) that support the [original Gameboy Printer](https://en.wikipedia.org/wiki/Game_Boy_Printer). The total cost for all the parts bougth new is below $15. All parts can be easily exchanged with other projects, reused or harvested from dead electronics as they are all very common.

You just need to upload the code using the Arduino IDE, connect the components like described here, plug your SD card and print any image directly from a Gameboy compatible game. Serial protocol is directly recorded under binary form on SD card. Once your printing session is finished, hold the [button](https://github.com/zenaro147/NeoGB-Printer/wiki/Hardware-Setup#push-button) for a few seconds and all the recorded data will be quickly converted to BMP and/or PNG images, ready to use with social media. Reboot the device once and you will access to an intuitive webserver mode.

## How to build one?
All the instructions can be found in our [Wiki Page](https://github.com/zenaro147/NeoGB-Printer/wiki)

Make sure to follow every step. If you still have questions, ask us here or in the [Game Boy Camera Club Discord](https://discord.gg/dKND7cFuqM)

## User manual in brief
![user_manual](/Supplementary_images/User_manual.png)

# Builds Showcases:
![My personal prototype build - zenaro147](/showcase/zenaro147.jpg)
![Setup by Raphaël BOICHOTy](/showcase/RaphaelBOICHOT.jpg)

### ⚠ Take care ⚠
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

Want to discuss with the authors or share your art and projects with people fond of the Game Boy Camera and Pocket Printer ? [**Join the Game Boy Camera Club Discord**](https://discord.gg/dKND7cFuqM)

![credits](/Supplementary_images/credits.png)

# List of homebrew projects compatible with the NeoGB Printer:
* [reini1305 - gb_snake](https://github.com/reini1305/gb_snake) - perfect emulation.
* [reini1305 - gb_bannerprinter](https://github.com/reini1305/gb_bannerprinter) - perfect emulation.
* [The-Arduino-SD-Game-Boy-Printer](https://github.com/Raphael-Boichot/The-Arduino-SD-Game-Boy-Printer) - it is convoluted but possible. It can be used as debug tool.
* [Blarble1290](https://8bittygames.com/blarble1290/) - it works if you convert the dumps between each dialog round, boot in server mode, read the message, boot in printer mode, print a new message, etc. The Game itself works with actual Game Boy Printers, not with Pocket Printers, which probably implies tricky timing issues within the protocol used.

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
