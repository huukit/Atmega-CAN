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

#define __CANBRIDGE_USE_QT

#ifdef __CANBRIDGE_USE_QT
#include <QObject>
#endif

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

    canBridgeDefinitions::errorCode init(uint32_t busSpeed);
    void close();

    canBridgeDefinitions::errorCode sendMessage(uint32_t id, uint8_t rtr, uint8_t len, uint8_t * data);
    uint32_t getMessage(time_t &drvtime, uint32_t &devtime, uint32_t &id, uint8_t &rtr, uint8_t &len, uint8_t * data);

    void registerCallback(rxIntCallback cb);

    std::string getLibraryVersionString();
signals:

private:
    bool initCalled;
};

#endif // LIBCANBRIDGE_H
