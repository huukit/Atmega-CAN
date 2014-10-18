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

#ifndef DEVICECOMMUNICATOR_H
#define DEVICECOMMUNICATOR_H

#include <QObject>
#include <stdint.h>

#include "libusb.h"
#include "canmessage.h"
#include <QVector>
#include <QDebug>

namespace deviceCommands{
    static const uint32_t initBus = 1;
    static const uint32_t closeBus = 2;
    static const uint32_t getCanData = 3;
    static const uint32_t sendCanData = 4;
    static const uint32_t getFreeTx = 5;

    #pragma pack(1)
    typedef struct _canMessage{
        uint32_t cantime;	/**< CAN message timestamp. */
        uint32_t mesid;		/**< CAN message ID. */
        uint8_t rtr;		/**< CAN message RTR flag. */
        uint8_t length;		/**< CAN message data length. */
        uint8_t data[8];	/**< CAN message data buffer. */
    } canMessage;
    #pragma pack()
}

static const uint8_t maxUsbControlDataLen = 8;
static const uint8_t maxUsbDataLen = 254;

typedef QVector<CanMessage *> canMessageBuffer_t;

// Helper class for handling device interrupts.
#include <QThread>
#include <QMutex>

static QMutex bufferMutex;

class DeviceInterruptHandler : public QThread{
    Q_OBJECT
public:

    DeviceInterruptHandler(libusb_device_handle * handle, canMessageBuffer_t * mbuf, QObject * parent = 0)
        : usbDeviceHandle(handle), messageBuffer(mbuf), QThread(parent){
        runEnabled = false;
    }

    void run(){
        runEnabled = true;
        ssize_t success = LIBUSB_SUCCESS;
        uint8_t dataControlBuffer[maxUsbControlDataLen];
        uint8_t dataBuffer[maxUsbDataLen];

        int32_t actDatalen;
        while(runEnabled){
            // Wait for interrupt.
            success = libusb_interrupt_transfer(usbDeviceHandle, LIBUSB_ENDPOINT_IN | 1, dataControlBuffer, maxUsbControlDataLen, &actDatalen, 1000);
            if(success >= LIBUSB_SUCCESS){
                for(uint32_t i = 0; i < dataControlBuffer[0]; i++){
                    success = libusb_control_transfer(usbDeviceHandle, LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_IN, deviceCommands::getCanData, 0, 0, dataBuffer, sizeof(deviceCommands::canMessage), 1000);
                    //qDebug("Has message.");
                    CanMessage * mes = new CanMessage();
                    deviceCommands::canMessage * devmsg = (deviceCommands::canMessage *)dataBuffer;
                    mes->insertData(devmsg->cantime, 0, devmsg->mesid, devmsg->rtr, devmsg->length, devmsg->data);
                    bufferMutex.lock();
                    messageBuffer->append(mes);
                    bufferMutex.unlock();
                }
                emit(hasMessages(dataControlBuffer[0]));

            }
        }
    }

    void stop(){
        runEnabled = false;
    }

signals:
    void hasMessages(int);

private:
    bool runEnabled;
    libusb_device_handle * usbDeviceHandle;
    canMessageBuffer_t * messageBuffer;
};

// Main communicator.
class DeviceCommunicator : public QObject
{
    Q_OBJECT
public:
    explicit DeviceCommunicator(libusb_device_handle * handle, QObject *parent = 0);

    bool sendOpenCommand(uint32_t busSpeed);
    bool sendCloseCommand();

    uint32_t getRxMessageBufferLength(){
        return messageBuffer.length();
    }

    bool getMessageFromBuffer(CanMessage & message);


    bool sendMessageCommand(CanMessage & message);

signals:
    void hasMessage(int);

public slots:
    void hasInt(int mcount);

private:
    libusb_device_handle * usbDeviceHandle;
    DeviceInterruptHandler * intHandler;
    canMessageBuffer_t messageBuffer;
};

#endif // DEVICECOMMUNICATOR_H
