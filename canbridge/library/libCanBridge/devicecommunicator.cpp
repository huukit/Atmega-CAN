#include "devicecommunicator.h"
#include <QDebug>

DeviceCommunicator::DeviceCommunicator(libusb_device_handle * handle, QObject *parent) :
    usbDeviceHandle(handle), QObject(parent)
{
    intHandler = new DeviceInterruptHandler(handle, this);
}

bool DeviceCommunicator::sendOpenCommand(uint32_t busSpeed){
    // Start interrupthandler before opening bus so no messages get missed.
    intHandler->start();
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
    intHandler->stop();
    intHandler->wait();
    ssize_t success = libusb_control_transfer(usbDeviceHandle, LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_IN, deviceCommands::closeBus, 0, 0, 0, 0, 1000);
    if(success != 0){
        qDebug() << "LibUSB error :" << success;
        return false;
    }
    return true;
}
