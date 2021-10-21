# NeoGB Printer

The main goal of Neo Printer is keep many simple as possible to the end user. Since the building to it's use.

This project is very similar to a popular solution available to buy, but the NeoGB Printer is an open-source and standalone Gameboy Printer emulator 100% compatible with [all officially released games (110 in total)](https://docs.google.com/spreadsheets/d/1RQeTHemyEQnWHbKEhUy16cPxR6vA3YfeBbyx2tIXWaU/edit#gid=0) that support the [original Gameboy Printer](https://en.wikipedia.org/wiki/Game_Boy_Printer)

You just need upload the code using the Arduino IDE, connect the components like described here, plug your SD card and print any image directly from a Gameboy compatible game

Once you finish to print all your images you want, hold the [button](#push-button-setup) (see below) for a few seconds and all your images will be converted to BMP image

## Software Setup
First of all, rename the `config.h.txt` to just `config.h` to import the pinout settings.

This file contains all the options that can be changed in the emulator. Customize it according to the board and modules you are using.

To install the ESP32 board for the Arduino IDE, follow the [instructions here](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html). I highly recommended follow the instructions to install via Boards Manager.

## Hardware Setup
This code has been created for a "DOIT ESP32 DEVKIT V1" [ESP32 based board](https://github.com/espressif/arduino-esp32/). All my tests was executed using [this chinese board](https://a.aliexpress.com/_mOCHLMT). You can use any other board available in the market. Just make sure of few points:\
* The number of total pins available in the board doesn't matter (Could be with 30 pins, 36 pins, 38 pins, whatever)
* It's a Dual Core module (some ESP32 modules are single core, like the ESP32-S2 and ESP32-C3)
* Have, at least, 2 SPI pins gourps [like this example](https://4.bp.blogspot.com/-nGLtB2nUrDg/Wp6DQbzcJMI/AAAAAAAABq0/A6Z46p0SQSEdERWocWL94oUmeATMQre4wCLcBGAs/s1600/3.png) (normally it's called HSPI and VSPI, or sometimes have one called SPI and the other called VSPI or HSPI), make sure to check the pinout before buy one;

Some boards already have a SD Card slot built in. I never tested using this kind of board, but IN THEORY should works fine (as long as it has SPI pins available). If you want to use this type of board, DO IT AT YOUR OWN RISK!

## Gameboy Link Cable Setup
Gameboy Original/Color Link Cable Pinout. If you don't want to sacrifice a Link Cable, you can use this [Gameboy Link Cable Breakout PCB](https://github.com/Palmr/gb-link-cable) to connect the pins and keep your Link Cable safe! [You can buy this board here, from OSH Park](https://oshpark.com/shared_projects/srSgm3Yj)
```
 __________
|  6  4  2 |
 \_5__3__1_/ (at cable)

| Link Cable |  ESP32  |
|------------|---------|
| Pin 1      |   N/A   | <- 5v 
| Pin 2      | G19     | <- Serial Out (Serial In at the other end of the cable)
| Pin 3      | G23     | <- Serial In (Serial Out at the other end of the cable)
| Pin 4      |   N/A   | <- Serial Data
| Pin 5      | G18     | <- Clock Out
| Pin 6      | G       | <- GND

```

## SD Card Reader Setup
You need to use a [Micro SD Card Module](https://pt.aliexpress.com/item/4000002592780.html) or a [SD Card Module](https://pt.aliexpress.com/item/32523666863.html) to save the data. I highly recommend to get one, especially the [SD Card Module](https://pt.aliexpress.com/item/32523666863.html), It's more stable than [Micro SD Card Module](https://pt.aliexpress.com/item/4000002592780.html), at least during my tests.
To use it, connect the pins following this schema
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
You need to add a [little Push Button like this](https://pt.aliexpress.com/item/1005002824489337.html) to convert all RAW data to BMP. To use it, just set the `#define BTN_PUSH` in `config.h` to any pin you want.

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
| 3 or 4 |  G34  | <-- I recommend to connect a 10K or 6K resistor to the GND together, to act as a Pull Down.

```

## RGB LED to display the Status (optional)
You can add a [simple RGB LED like this](https://pt.aliexpress.com/item/1005002535018824.html). This LED will be very useful to display the printer status, like Idle, Converting, Receiving Data, etc.

To use it, you need to uncomment `#define COMMON_ANODE` or `#define COMMON_CATHODE`, based on your LED. Edit the other legs in the `config.h` based on your setup.
If your LED uses common Anode, connect it to the 3.3v Pin. If it's common Cathode, connect it to the GND.
For the other legs, you need to use at least a 22Oohm Resistor on each RGB leg. Connect then following the example schema below (always based on your `config.h` file
```
| LED | ESP32 |
|-----|-------|
|  R  |  16   | <-- YOU CAN USE ANY GPIO AVAILABLE. Connect at least a 220ohm Resistor with it.
|  G  |   4   | <-- YOU CAN USE ANY GPIO AVAILABLE. Connect at least a 220ohm Resistor with it.
|  B  |  17   | <-- YOU CAN USE ANY GPIO AVAILABLE. Connect at least a 220ohm Resistor with it.

```

## OLED Display Setup (optional)
You can add a [tiny oled display like this](https://pt.aliexpress.com/item/32672229793.html). To use it, you need to uncomment `#define USE_OLED` and the following lines   
The display will show the current status of the printer.
```
| OLED DISPLAY |   ESP32   |
|--------------|-----------|
| GND          | GND       | 
| VIN          | 5v or 3v3 |
| SCL          | G22       | <-- YOU CAN USE ANY GPIO AVAILABLE
| SDA          | G21       | <-- YOU CAN USE ANY GPIO AVAILABLE

```

# Builds Showcases:
![My personal prototype build - zenaro147](/showcase/zenaro147.jpg)
![Setup by Raphaël BOICHOTy](/showcase/RaphaelBOICHOT.jpg)

## ⚠ Take care ⚠
You should not power the ESP from the GameBoy, as this might damage the GameBoy itself.

# Based on existing projects:
* [Mofosyne - Arduino Gameboy Printer Emulator](https://github.com/mofosyne/arduino-gameboy-printer-emulator)
* [HerrZatacke - WiFi Game Boy Printer Emulator](https://github.com/HerrZatacke/wifi-gbp-emulator)
* [Raphaël BOICHOT - Gameboy Printer Paper Simulation](https://github.com/Raphael-Boichot/GameboyPrinterPaperSimulation)

# ToDo List:
- [x] Update the code to support all games
- [x] Use the original BMP library from Mofosyne C decompiler
- [X] Write the BMP file on SD
- [X] Fix Palette issue with some games
- [X] Handle with storage % instead number of files
- [X] Parse the Output directory too, to return the next image ID
- [X] Update documetation (wiring, compatible devices, etc)
- [X] Improve the LED status using a RGB LED
- [ ] Add support to PNG instead BMP
- [ ] Add a message to display the number of images printed
