#ifndef DEVICECOMMUNICATOR_H
#define DEVICECOMMUNICATOR_H

#include <QObject>
#include <stdint.h>

#include "libusb.h"

namespace deviceCommands{
    static const uint32_t initBus = 1;
    static const uint32_t closeBus = 2;
    static const uint32_t getCanData = 3;
    static const uint32_t sendCanData = 4;

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

#include <QThread>

class DeviceInterruptHandler : public QThread{
    Q_OBJECT
public:
    DeviceInterruptHandler(libusb_device_handle * handle, QObject * parent = 0)
        : usbDeviceHandle(handle), QThread(parent){
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
                    qDebug("Has message.");
                }
                emit(dataControlBuffer[0]);
            }
        }
    }

    void stop(){
        runEnabled = false;
    }

signals:
    void hasMessages(uint8_t);

private:
    bool runEnabled;
    libusb_device_handle * usbDeviceHandle;
};

class DeviceCommunicator : public QObject
{
    Q_OBJECT
public:
    explicit DeviceCommunicator(libusb_device_handle * handle, QObject *parent = 0);

    bool sendOpenCommand(uint32_t busSpeed);
    bool sendCloseCommand();

signals:

public slots:

private:
    libusb_device_handle * usbDeviceHandle;
    DeviceInterruptHandler * intHandler;
};

#endif // DEVICECOMMUNICATOR_H
