# The NeoGB Printer - ポケットプリンタエミュレータ

<a href="https://www.buymeacoffee.com/zenaro147" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-yellow.png" alt="Buy Me A Coffee" style="height: 60px !important;width: 217px !important;" ></a>

The NeoGB Printer is a SD card-based standalone Game Boy Printer emulator. It is easy to build with parts readily available on the market. It does require little to no soldering skill and is fast to install from Arduino IDE (few dependancies). Using it is straigthforward and intuitive. Mounting the device with all parts on a table requires about an afternoon from opening the component bags to printing.

This project is very similar to a popular ready-to-use solution available on the market, but the NeoGB Printer is open-source, cheap and tested with success with all officially released games (110 in total) that support the [original Gameboy Printer](https://en.wikipedia.org/wiki/Game_Boy_Printer). The total cost for all the parts bougth new is below $15. All parts can be easily exchanged with other projects, reused or harvested from dead electronics as they are all very common.

You just need to upload the code using the Arduino IDE, connect the components like described here, plug your SD card and print any image directly from a Gameboy compatible game. Serial protocol is directly recorded under binary form on SD card. Once your printing session is finished, hold the [button](https://github.com/zenaro147/NeoGB-Printer/wiki/Hardware-Setup#push-button) for a few seconds and all the recorded data will be quickly converted to BMP and/or PNG images, ready to use with social media. Reboot the device once and you will access to an intuitive webserver mode.

[**List of Games/Homebrew projects compatible with the NeoGB Printer**](https://github.com/zenaro147/NeoGB-Printer/wiki/Compatible-Games)

## How to build one?
All the instructions can be found in our **Wiki Page** [![Wiki](https://img.shields.io/badge/docs-on%20Wiki-lightgrey)](https://github.com/zenaro147/NeoGB-Printer/wiki)

## Development Status
Latest Stable Release  [![Release Version](https://img.shields.io/github/v/release/zenaro147/NeoGB-Printer?style=plastic)](https://github.com/zenaro147/NeoGB-Printer/releases/latest/)  [![Release Date](https://img.shields.io/github/release-date/zenaro147/NeoGB-Printer?style=plastic)](https://github.com/zenaro147/NeoGB-Printer/releases/latest/)

Latest Development Release  [![Release Version](https://img.shields.io/github/release/zenaro147/NeoGB-Printer/all.svg?style=plastic)](https://github.com/zenaro147/NeoGB-Printer/releases/) [![Release Date](https://img.shields.io/github/release-date-pre/zenaro147/NeoGB-Printer.svg?style=plastic)](https://github.com/zenaro147/NeoGB-Printer/releases/) 

Want a PCB for a cleaner design and easy to build? Check my [**PCB project here**](/NeoGB_Printer_PCB) !

If you still have questions, ask us here or in the **Game Boy Camera Club Discord** [![Discord Group](https://img.shields.io/badge/chat-on%20Discord-738ADB)](https://discord.gg/dKND7cFuqM)

## User manual in brief
![user_manual](/Supplementary_images/User_manual.png)

## Builds Showcases:
![My personal prototype build - zenaro147](/showcase/zenaro147.jpg)
![Setup by Raphaël BOICHOT](/showcase/RaphaelBOICHOT.jpg)
![Setup by Cristofer Cruz](/showcase/cristofercruz.jpg)

## Posts about:
* [Hack a Day Article](https://hackaday.com/2021/10/22/an-open-source-game-boy-printer-that-doesnt-print/)
* [Reddit post](https://www.reddit.com/r/Gameboy/comments/qdfoys/the_first_opensource_and_fully_standalone_game/)
* [Distant Arcade - Blog post](https://distantarcade.co.uk/the-neogb-printer-game-boy-printer-emulator/)

## Authors contribution:
* Rafael Zenaro: main code, hardware setting, new ideas, concept art, technical innovations and group facilitation.
* Brian Khuu: architect of the Matrix, Game Boy Printer emulator core, BMP image decoder core.
* Raphaël Boichot: code debugging, pixel perfect upscalers, blinking stuff support, hardcore gaming with Japanese kusoge.
* Cristofer Cruz: 3D model for the GB Printer shell, code adaptation to LilyGo TTGO T2 Board.
* Herr_Zatacke: support for the Game Boy Printer Web compatibility.

Want to discuss with the authors or share your art and projects with people fond of the Game Boy Camera and Pocket Printer? [![Discord Group](https://img.shields.io/badge/chat-on%20Discord-738ADB)](https://discord.gg/dKND7cFuqM)

![credits](/Supplementary_images/credits.png)

## Based on existing projects:
* [Mofosyne - Arduino Gameboy Printer Emulator](https://github.com/mofosyne/arduino-gameboy-printer-emulator)
* [HerrZatacke - WiFi Game Boy Printer Emulator](https://github.com/HerrZatacke/wifi-gbp-emulator)
* [Raphaël BOICHOT - Gameboy Printer Paper Simulation](https://github.com/Raphael-Boichot/GameboyPrinterPaperSimulation)
