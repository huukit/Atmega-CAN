# This is the public repository for proximia.fi.
Any code here is licensed under GPL/LGPL.

## Projects:
### Can bootloader
This is a simple and straightforward bootloader for atmega devices with CAN support. It uses as simlpified CANOpen protocol to flash a program file and/or write bytes to the EEPROM of the device. Up-to-date documentation can be generated from the attached source files with doxygen. An online version of the documentation is available at: http://proximia.fi/doc/canbootloader/
The basic usage can be found in the documentation.

### CAN library for AVR devices (under lib)
A user friendly CAN library to use with AVR devices. Uses the Atmel driver for low level interactions. Documentation can be generated with doxygen or the online version viewed here: http://proximia.fi/doc/canlibrary/

### Libraries
Any generic libraries used by the projects are included here. You will need these for the bootloader (CAN driver, UART for debugging) and the CAN-USB converter.
Folders:
/lib
-/uart      UART library for ATmega64C1 and similar.
-/can       The CAN library (ATmega64C1, AT90CAN32/64/128 and similar)
-/3rdparty
--/usbdrv   Vusb-driver for the CAN-USB converter (ATmega64C1) (https://www.obdev.at/products/vusb/index.html)

### Can Bridge CAN-USB converter
Proof-of-concept CAN-USB converter made with the least components available. The schematic can be found under the schematics-folder. 
The project includes the following parts:
* Hardware under schematics.
* Firmware for processor under canbridge/embedded (you will need the CAN- and USB-drivers mentioned above.)
* Driver files for windows under canbridge/driver.
* Documented API written with C++ and including comileable QT support.
  * buildable library module (.so, .dll)
  * libusb for usb-connectivity on the PC side.
  * a tester application for the whole chain above written in QT.
This project can also be used as a reference for small microcontrollers wanting to communicate with devices over USB.

### Schematics
Schematics for the CAN-USB converter. Also includes schematics for a really simple UART-USB converter that has been made somewhat obsolete by the cheap ones on ebay and similar sites.

### Example usage
The projects above can be used to build a distributed can measurement network, where nodes have can have arbitrary amounts and types of sensors. The nodes can then send data to the bus and that data can be viewed on the computer using the CAN-USB bridge. Flashing the nodes with the bootloader enables the updating of any node on the network. 
This is actually what this project is actually been used for at the moment, but as conciderable work has gone in the the implementation of all the measurement nodes, the source is not available to the public.

### Feedback
Please send any feedback to tuomas.huuki[at]proximia.fi