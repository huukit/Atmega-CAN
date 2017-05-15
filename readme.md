# This is the public repository for proximia.fi.
Any code here is licensed under GPL/LGPL.

## Projects: 
### Can bootloader
This is a simple and straightforward bootloader for atmega devices with CAN support. It uses as simlpified CANOpen protocol to flash a program file and/or write bytes to the EEPROM of the device. Up-to-date documentation can be generated from the attached source files with doxygen. An online version of the documentation is available at: http://proximia.fi/doc/canbootloader/

### CAN library for AVR devices (under lib)
A user friendly CAN library to use with AVR devices. Uses the Atmel driver for low level interactions. Documentation can be generated with doxygen or the online version viewed here: http://proximia.fi/doc/canlibrary/

### Libraries
Any generic libraries used by the projects are included here. You will need these for the bootloader (CAN driver, UART for debugging) and the CAN-USB converter.

### Can Bridge CAN-USB converter
Proof-of-concept CAN-USB converter made with the least components available.

### Schematics
Schematics for the CAN-USB converter. Also includes schematics for a really simple UART-USB converter.

For more information, please browse the project documentation.
Documentation can be generated from the sources with Doxygen.
