/*
 * <copyright>
 * Copyright (c) 2014: Tuomas Huuki / Proximia, http://proximia.fi/
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Lesser GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <?http://www.gnu.org/licenses/>.
 * </copyright>
 */

/*
 NOTE -- NOTE -- NOTE -- NOTE -- NOTE -- NOTE -- NOTE -- NOTE -- NOTE -- NOTE
 This is a proof-of-concept implementation and only intended as an example of implementing
 a library for a USB-CAN converter.
*/

#ifndef LIBCANBRIDGE_H
#define LIBCANBRIDGE_H

#if defined(LIBCANBRIDGE_LIBRARY)
#  define LIBCANBRIDGESHARED_EXPORT __declspec(dllexport)
#else
#  define LIBCANBRIDGESHARED_EXPORT __declspec(dllimport)
#endif

#include <string>
#include <stdint.h>
#include <time.h>

/* Define this to use slots and sockets in qt, otherwise just use the callback.
    And yes, obviously you will need to recompile the whole library if you change this.
*/
#define __CANBRIDGE_USE_QT

#ifdef __CANBRIDGE_USE_QT
#include <QObject>
#endif

// Library definitions.
namespace canBridgeDefinitions{
    static const std::string libraryvendor = "http://proximia.fi";
    static const std::string libraryname = "libCanBridge";
    static const uint8_t libraryversion[3] = {0, 1, 'a'};

    enum errorCode{
        // OK
        errOk = 0,

        // No such device, or device removed
        errNoDevice = -1,

        // LibUsb related errors
        errUsbInitFailed = -20,
        errUsbInitNoDevices = -21,
        errUsbInitInvalidName = -22,
        errUsbInitNoChannel = -23,

        // Library internal errors.
        errInvalidSpeed = -50,

        // Device communication errors
        errCannotOpenDevice = -60,
        errRxBufferEmpty = -65,
        errTxQueueFull = -66
    };

    enum busSpeeds{
        bus250k = 250000
    };
}

// Callback when messages are ready. Parameter is the number of messages waiting in the buffer.
typedef void(* rxIntCallback)(uint32_t);

class LIBCANBRIDGESHARED_EXPORT LibCanBridge
#ifdef __CANBRIDGE_USE_QT
: public QObject
{
   Q_OBJECT
#else
{
#endif

public:

#ifdef __CANBRIDGE_USE_QT
    explicit LibCanBridge(QObject *parent = 0);
#else
    LibCanBridge();
#endif
    ~LibCanBridge();

    // Init the library and open a connection to the device.
    canBridgeDefinitions::errorCode init(uint32_t busSpeed);
    // Close the connection to the device.
    void close();

    // Send a can mesage.
    canBridgeDefinitions::errorCode sendMessage(uint32_t id, uint8_t rtr, uint8_t len, uint8_t * data);
    // The driver constantly reveives messagesd from the device, this function gets the message from the internal driver buffer.
    uint32_t getMessage(time_t &drvtime, uint32_t &devtime, uint32_t &id, uint8_t &rtr, uint8_t &len, uint8_t * data);

    // If you do not use the QT slots & signals, you may use this to register a callback.
    void registerCallback(rxIntCallback cb);

    // Return the library version as well as the libusb version it was compiled with.
    std::string getLibraryVersionString();

#ifdef __CANBRIDGE_USE_QT
signals:
    // Signal to emit when there are messages waiting in the buffer. Parameter is number of messages in buffer.
    void hasMessage(int);
#endif

private:
    bool initCalled;
};

#endif // LIBCANBRIDGE_H
