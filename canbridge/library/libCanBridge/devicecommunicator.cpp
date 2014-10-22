#include "devicecommunicator.h"
#include <QDebug>

static const uint32_t maxTxRetries = 10;

DeviceCommunicator::DeviceCommunicator(libusb_device_handle * handle, QObject *parent) :
    usbDeviceHandle(handle), QObject(parent)
{
    intHandler = new DeviceInterruptHandler(handle, &messageBuffer, this);
}

void DeviceCommunicator::hasInt(int mcount){
    //qDebug() << "Has message, buflen: " << mcount;
    emit hasMessage(mcount);

}

bool DeviceCommunicator::sendMessageCommand(CanMessage &message){
    uint8_t usbDataBuffer[maxUsbControlDataLen];
    uint32_t i;
    ssize_t success = 0;

    for(i = 0; i < maxTxRetries; i++){
        success = libusb_control_transfer(usbDeviceHandle, LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_IN, deviceCommands::getFreeTx, 0, 0, usbDataBuffer, 8, 1000);
        if(success != 1)
            return false;
        // See if we have free buffer (0xFF = no free buffers).
        if(usbDataBuffer[0] != 0xFF)
            break;
    }
    // Retries up, return send failure.
    if(i == maxTxRetries)
        return false;
    // We have free buffer, send data.
    deviceCommands::canMessage msg;
    msg.mesid = message.getId();
    msg.rtr = message.getRtr();
    msg.length = message.getLen();
    memcpy(msg.data, message.getData(), message.getLen());

    success = libusb_control_transfer(usbDeviceHandle, LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_OUT, deviceCommands::sendCanData, 0, 0, (uint8_t *)&msg, sizeof(deviceCommands::canMessage), 1000);
    if(success < LIBUSB_SUCCESS)
        return false;
    return true;
}

bool DeviceCommunicator::sendOpenCommand(uint32_t busSpeed){
    // Start interrupthandler before opening bus so no messages get missed.
    intHandler->start();
    connect(intHandler, SIGNAL(hasMessages(int)), this, SLOT(hasInt(int)));

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

    disconnect(intHandler, SIGNAL(hasMessages(int)), this, SLOT(hasInt(int)));

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
    bufferMutex.lock();
    message = *messageBuffer.first();
    messageBuffer.pop_front();
    bufferMutex.unlock();
    return true;
}
