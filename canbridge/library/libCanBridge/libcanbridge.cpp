#include "libcanbridge.h"
#include "libusb.h"
#include "devicecommunicator.h"

#include <QString>
#include <QDebug>
#include "signaltrampoline.h"

namespace canBridgeInternals{
    // USB library.
    static libusb_device_handle *currentdevhndl = 0;

    const uint16_t usbDevVID = 0x16C0;
    const uint16_t usbDevPID = 0x05DC;
    std::string usbDevVendorString = "proximia.fi";
    std::string usbDevDeviceString = "CANBridge";
    const uint32_t usbDevMaxDescrLength = 1024;
    const uint8_t usbLibraryContext = NULL;

    // Communicator.
    static DeviceCommunicator * deviceCom = 0;
    static SignalTrampoline * sigTrampoline = 0;
}

LibCanBridge::LibCanBridge()
{
    canBridgeInternals::deviceCom = 0;
    canBridgeInternals::sigTrampoline = new SignalTrampoline();
    initCalled = false;
}

LibCanBridge::~LibCanBridge(){
    close();
    delete canBridgeInternals::sigTrampoline;
}

void LibCanBridge::registerCallback(rxIntCallback cb){
    canBridgeInternals::sigTrampoline->intCallback = cb;
}

std::string LibCanBridge::getLibraryVersionString(){
    return canBridgeDefinitions::libraryvendor + "\n" +\
            canBridgeDefinitions::libraryname + " version: " + QString("%1.%2.%3\n").arg(canBridgeDefinitions::libraryversion[0])\
                                                                            .arg(canBridgeDefinitions::libraryversion[1])\
                                                                            .arg((char)canBridgeDefinitions::libraryversion[2]).toStdString() +\
                             QString("Linked with libusb version: %1.%2.%3").arg(libusb_get_version()->major)\
                                                                            .arg(libusb_get_version()->minor)\
                                                                            .arg(libusb_get_version()->micro).toStdString();
}

void LibCanBridge::close(){
    if(initCalled){
        canBridgeInternals::deviceCom->sendCloseCommand();
        libusb_close(canBridgeInternals::currentdevhndl);
        libusb_exit(canBridgeInternals::usbLibraryContext);
        initCalled = false;
    }
    if(canBridgeInternals::deviceCom){
        delete canBridgeInternals::deviceCom;
        canBridgeInternals::deviceCom = 0;
    }
}

canBridgeDefinitions::errorCode LibCanBridge::init(uint32_t busSpeed){
    if(busSpeed != canBridgeDefinitions::bus250k)
        return canBridgeDefinitions::errInvalidSpeed;

    // Variables for libusb
    ssize_t success = LIBUSB_SUCCESS;
    libusb_device **list;
    libusb_device *found = NULL;
    struct libusb_device_descriptor desc;

    // Init usb library.
    success = libusb_init(canBridgeInternals::usbLibraryContext);
    if(success != LIBUSB_SUCCESS){
        qDebug() << "Libusb init failed.";
        return canBridgeDefinitions::errUsbInitFailed;
    }

    initCalled = true;

    // Get list of devices.
    success = libusb_get_device_list(canBridgeInternals::usbLibraryContext, &list);

    if(success < LIBUSB_SUCCESS){
        qDebug() << "USB device discovery failure.";
        return canBridgeDefinitions::errUsbInitNoDevices;
    }

    // Go through list of devices and find correct PID/VID
    uint32_t status;
    ssize_t i = 0;
    for(i = 0; i < success; i++){
        libusb_device *device = list[i];
        status = libusb_get_device_descriptor(device, &desc);
        if(status != LIBUSB_SUCCESS){
            return canBridgeDefinitions::errUsbInitNoDevices;
        }
        qDebug("Vendor 0x%0.4X product 0x%0.4X", desc.idVendor, desc.idProduct);
        if(desc.idVendor = canBridgeInternals::usbDevVID && desc.idProduct == canBridgeInternals::usbDevPID){
            qDebug() << "Found device with correct vid/pid";
            found = device;
            break;
        }
    }

    // Open device to get the descriptors.
    status = libusb_open(found, &canBridgeInternals::currentdevhndl);

    char description[canBridgeInternals::usbDevMaxDescrLength];

    // Make sure device descriptors are correct.
    libusb_get_string_descriptor_ascii(canBridgeInternals::currentdevhndl, desc.iProduct, (unsigned char *) description, canBridgeInternals::usbDevMaxDescrLength);
    if(strcmp(description, canBridgeInternals::usbDevDeviceString.c_str()))
        return canBridgeDefinitions::errUsbInitInvalidName;

    libusb_get_string_descriptor_ascii(canBridgeInternals::currentdevhndl, desc.iManufacturer, (unsigned char *) description, canBridgeInternals::usbDevMaxDescrLength);
    if(strcmp(description, canBridgeInternals::usbDevVendorString.c_str()))
        return canBridgeDefinitions::errUsbInitInvalidName;

    qDebug() << "Vendor string is: "<< QString::fromLatin1(description);

    // Enable endpoint 1
    success = libusb_set_configuration(canBridgeInternals::currentdevhndl, 1);
    if(success != LIBUSB_SUCCESS)
        return canBridgeDefinitions::errUsbInitNoChannel;

    // Finally claim the interface.
    libusb_claim_interface(canBridgeInternals::currentdevhndl, 0);

    qDebug() << "LibUsb init complete.";
    canBridgeInternals::deviceCom = new DeviceCommunicator(canBridgeInternals::currentdevhndl, 0);

    if(!canBridgeInternals::deviceCom->sendOpenCommand(canBridgeDefinitions::bus250k))
        return canBridgeDefinitions::errCannotOpenDevice;

    // Free the device list.
    return canBridgeDefinitions::errOk;
}

uint32_t LibCanBridge::getMessage(time_t drvtime, uint32_t devtime, uint32_t &id, uint8_t &rtr, uint8_t &len, uint8_t * data){
    if(canBridgeInternals::deviceCom->getMessageBufferLength() == 0)
        return canBridgeDefinitions::errRxBufferEmpty;

    CanMessage msg;
    canBridgeInternals::deviceCom->getMessageFromBuffer(msg);

    id = msg.getId();
    rtr = msg.getRtr();
    len = msg.getLen();
    memcpy(data, msg.getData(), len);
    devtime = msg.getRevTimeDriverMs();
    drvtime = msg.getTimeRaw();
    return canBridgeInternals::deviceCom->getMessageBufferLength();
}

canBridgeDefinitions::errorCode LibCanBridge::sendMessage(uint32_t id, uint8_t rtr, uint8_t len, uint8_t *data){
    return canBridgeDefinitions::errTxQueueFull;
}
