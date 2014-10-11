#ifndef LIBCANBRIDGE_H
#define LIBCANBRIDGE_H

#if defined(LIBCANBRIDGE_LIBRARY)
#  define LIBCANBRIDGESHARED_EXPORT __declspec(dllexport)
#else
#  define LIBCANBRIDGESHARED_EXPORT __declspec(dllimport)
#endif

#include <string>
#include <stdint.h>

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
        errInvalidSpeed = -50
    };

    enum busSpeeds{
        bus250k = 250000
    };
}

class LIBCANBRIDGESHARED_EXPORT LibCanBridge
{

public:
    LibCanBridge();
    ~LibCanBridge();

    canBridgeDefinitions::errorCode init(uint32_t busSpeed);
    void close();
    std::string getLibraryVersionString();

private:

};

#endif // LIBCANBRIDGE_H
