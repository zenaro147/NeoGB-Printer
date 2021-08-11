# Yet Another GBP Emulator (NeoGB Printer)
Yes, it's another Game Boy Printer emulator! The idea is just plug an SD card, print your images using a game compatible with Gameboy Printer and save as BMP image (big goal, but for the first version, it will be a PGM file)

Currently the code only generates a PGM output (with a fixed header to Gameboy Camera) and only if you push a button (pin G34)
If you want to use with another game, just change the 144 in the header to the number of lines in the output (without the first 3 lines)

You can check the compatibility list here: [Game Boy Printer Emulator - Games Support](https://docs.google.com/spreadsheets/d/1RQeTHemyEQnWHbKEhUy16cPxR6vA3YfeBbyx2tIXWaU/edit#gid=0)

# ToDo List:
- [x] Update the code to support all games
- [ ] Parse Palette present in Print Command
- [ ] Merge files with multiple Print Commands (and palettes) properly
- [ ] Create the file automatically when the emulator finishes to receive all the data
- [ ] Write the PGM file on SD
- [ ] Change the image output from PGM to BMP

# Based on:
* [Mofosyne - Arduino Gameboy Printer Emulator](https://github.com/mofosyne/arduino-gameboy-printer-emulator)
* [HerrZatacke - WiFi Game Boy Printer Emulator](https://github.com/HerrZatacke/wifi-gbp-emulator)
* [Raphaël BOICHOT - Gameboy Printer Paper Simulation](https://github.com/Raphael-Boichot/GameboyPrinterPaperSimulation)
