#include "libcanbridge.h"
#include "libusb.h"
#include <QString>

LibCanBridge::LibCanBridge()
{

}

std::string LibCanBridge::getLibraryVersionString(){
    return "Proximia.fi CanBridge library version: " + QString("%1.%2.%3\n").arg(canBridgeDefinitions::libraryversion[0])\
                                                                             .arg(canBridgeDefinitions::libraryversion[1])\
                                                                             .arg((char)canBridgeDefinitions::libraryversion[2]).toStdString() +\
                             QString("Linked with libusb version. %1.%2.%3").arg(libusb_get_version()->major)\
                                                                             .arg(libusb_get_version()->minor)\
                                                                             .arg(libusb_get_version()->micro).toStdString();
}

canBridgeDefinitions::errorCode LibCanBridge::init(uint32_t busSpeed){
    if(busSpeed != canBridgeDefinitions::bus250k)
        return canBridgeDefinitions::errInvalidSpeed;

    return canBridgeDefinitions::errOk;
}
