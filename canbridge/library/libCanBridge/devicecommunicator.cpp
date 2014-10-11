#include "devicecommunicator.h"

DeviceCommunicator::DeviceCommunicator(libusb_device_handle * handle, QObject *parent) :
    usbBeviceHandle(handle), QObject(parent)
{
}
