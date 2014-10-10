#include "libcanbridge.h"
#include "libusb.h"

LibCanBridge::LibCanBridge()
{

}

QString LibCanBridge::getLibraryVersionString(){
    return "Proximia.fi CanBridge library. " + QString("Linked with libusb version. %1.%2.%3").arg(libusb_get_version()->major)\
                                               .arg(libusb_get_version()->minor)\
                                               .arg(libusb_get_version()->micro);
}
