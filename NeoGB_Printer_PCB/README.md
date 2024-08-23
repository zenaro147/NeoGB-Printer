# Gameboy Printer Emulator - ESP32 PCB

<a href="https://www.buymeacoffee.com/zenaro147" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-yellow.png" alt="Buy Me A Coffee" style="height: 60px !important;width: 217px !important;" ></a>

A simple PCB design using EasyEDA to work with this projects by me:
* [NeoGB Printer](https://github.com/zenaro147/NeoGB-Printer)
* [WiFi GBP Emulator - ESP32 Version](https://github.com/zenaro147/wifi-gbp-emulator/tree/feature/v3-esp32)

[Order from OSH Park](https://oshpark.com/shared_projects/Y6LBKwvG)

## BOM list 
| Component | Where to buy |
| --- | --- |
| ESP32 Dev Board (38 pins, this [exact board](/NeoGB_Printer_PCB/ESP32_used.png)) | [AliExpress](https://www.aliexpress.com/item/1005001636295529.html) |
| OLED Display 128x32 (0.91 inch) (optional) | [AliExpress](https://a.aliexpress.com/_mOHbftG) |
| SD Card/MicroSD Card Module (Choose only one! I recommend the Standard SD module) | [AliExpress - SD Card](https://a.aliexpress.com/_mMWz2km) / [AliExpress - MicroSD](https://a.aliexpress.com/_m0SS4P4) |
| 4 channel Logical Level Shifter (highly recommended) | [AliExpress](https://a.aliexpress.com/_m07pFpU) |
| Female Pin Header with 40 pins (you need at least 38 pins, or 2 with 20pins) | [AliExpress](https://fr.aliexpress.com/item/4000873858801.html) |
| PushButton 4.5x4.5mm (any height) - ONLY REV. 1  | [AliExpress](https://www.aliexpress.com/item/1005001629344310.html) |
| PushButton 6x6mm (any height) - ONLY REV. 2 (still W.I.P.) | [AliExpress](https://www.aliexpress.com/item/1005003251295065.html) |
| 5k or 10k resistor | [AliExpress](https://www.aliexpress.com/item/4001082088353.html) |
| MicroUSB Female (optional, but mandatory if you want the charging module)| [AliExpress](https://www.aliexpress.com/item/4000385426649.html) |
| Charging Module with StepUp (5V) - DD05CVSA (optional) | [AliExpress](https://www.aliexpress.com/item/33034500618.html) |
| Micro Switch - SS-12D00G3 (optional, but mandatory if you want the charging module) | [AliExpress](https://www.aliexpress.com/item/1005003938856402.html) |
| 2000mah Li-Po Battery (optional, but mandatory if you want the charging module) | [AliExpress](https://fr.aliexpress.com/item/1005002919536938.html) |
| AGB Link Port Connector (optional) | [AliExpress](https://fr.aliexpress.com/item/1005006375410306.html) |
| RGB LED 5mm (optional) | [AliExpress](https://www.aliexpress.com/item/1005002535018824.html) |
| 220 Ohms resistor (if you want the LED) | [AliExpress](https://www.aliexpress.com/item/4001082088353.html) |

## How to build
* First of all, solder the resistors, RGB led, button, the level shifter, USB port, and the female pin bar (oh, and the AGB link port, if you have one)
* Put a little bit of in the RGB led jumper to bridge the middle pad to one of the other pads. (If you want a passthrough USB power, make bridge pads too)
* Cut all remaining legs
* Solder the SD/Micro SD module
* Cut all remaining legs
* Solder the male pin head for the charging module (but don't solder the module yet)
* Cut all remaining legs
* Solder the display and the battery
* Cut all remaining legs
* Solder the charging module into the pins

I highly recommend to add some tape to prevent any unwanted contact between the modules (on SD/MicroSD, charging module and display to be more specific) and the solder points

## Inspirations/References
* [Game Boy -related custom hardware by gekkio](https://github.com/Gekkio/gb-hardware)
* [Game Boy WiFi Printer - D1 Mini Shield](https://github.com/cristofercruz/gbp-esp-shield-pcb)
* [Gameboy Link Cable Breakout PCB](https://github.com/Palmr/gb-link-cable)

## External References
* [ESP32-DEVKITC-32D](https://www.snapeda.com/parts/ESP32-DEVKITC-32D/Espressif%20Systems/view-part/)
* [ESP32 Pinout](https://4.bp.blogspot.com/-nGLtB2nUrDg/Wp6DQbzcJMI/AAAAAAAABq0/A6Z46p0SQSEdERWocWL94oUmeATMQre4wCLcBGAs/s1600/3.png)
* [ESP32 dev kit power options](https://techexplorations.com/guides/esp32/begin/power/)
* [Power Supply for ESP32 with Battery Charger & Boost Converter](https://how2electronics.com/power-supply-for-esp32-with-boost-converter-battery-charger/#37V_to_5V_Step-Up_Boost_Converter_Module)
* [ESP32 – Getting Battery charging level](https://www.pangodream.es/esp32-getting-battery-charging-level)
