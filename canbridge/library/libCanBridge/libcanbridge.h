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
    static const uint8_t libraryversion[3] = {0, 1, 'a'};

    enum errorCode{
        errOk = 0,
        errNoDevice = -1,
        errInvalidSpeed = -2
    };

    enum busSpeeds{
        bus250k = 250000
    };
}

class LIBCANBRIDGESHARED_EXPORT LibCanBridge
{

public:
    LibCanBridge();
    canBridgeDefinitions::errorCode init(uint32_t busSpeed);
    void close();
    std::string getLibraryVersionString();

};

#endif // LIBCANBRIDGE_H
