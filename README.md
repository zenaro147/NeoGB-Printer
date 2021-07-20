# Yet-Another-GBP-Emulator
Yes... it's another Game Boy Printer emulator... just plug an SD card, print your files and save the BMP image (big goal)

Currently the code only generates a PGM output (with a fixed header to Gameboy Camera) and only if you push a button (pin G34)
If you want to use with another game, just change the 144 in the header to the number of lines in the output (without the first 3 lines)

# ToDo List:
* Write the PGM file on SD
* Write automatically (maybe when the printer timed out)
* Create the BMP file instead PGM

# Based on:
* [Mofosyne - Arduino Gameboy Printer Emulator](https://github.com/mofosyne/arduino-gameboy-printer-emulator)
* [HerrZatacke - WiFi Game Boy Printer Emulator](https://github.com/HerrZatacke/wifi-gbp-emulator)
