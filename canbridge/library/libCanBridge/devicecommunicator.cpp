#include "devicecommunicator.h"
#include <QDebug>

DeviceCommunicator::DeviceCommunicator(libusb_device_handle * handle, QObject *parent) :
    usbDeviceHandle(handle), QObject(parent)
{
    intHandler = new DeviceInterruptHandler(handle, &messageBuffer, this);
}

bool DeviceCommunicator::sendOpenCommand(uint32_t busSpeed){
    // Start interrupthandler before opening bus so no messages get missed.
    intHandler->start();

    // Send start command, speed as data.
    uint8_t usbDataBuffer[maxUsbControlDataLen];
    memcpy(usbDataBuffer, reinterpret_cast<void *>(&busSpeed), sizeof(busSpeed));
    ssize_t success = libusb_control_transfer(usbDeviceHandle, LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_IN, deviceCommands::initBus, 0, 0, usbDataBuffer, sizeof(uint32_t), 1000);
    if(success < LIBUSB_SUCCESS){
        return false;
        qDebug() << "LibUSB error :" << success;
    }
    return true;
}

bool DeviceCommunicator::sendCloseCommand(){
    // Send close command.
    ssize_t success = libusb_control_transfer(usbDeviceHandle, LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_IN, deviceCommands::closeBus, 0, 0, 0, 0, 1000);

    // Stop interrupt & receive.
    intHandler->stop();
    intHandler->wait();
    while(!messageBuffer.isEmpty()){
        delete messageBuffer.last();
        messageBuffer.pop_back();
    }

    if(success != 0){
        qDebug() << "LibUSB error :" << success;
        return false;
    }
    return true;
}

bool DeviceCommunicator::getMessageFromBuffer(CanMessage &message){
    if(messageBuffer.isEmpty())return false;
    message = *messageBuffer.first();
    return true;
}
