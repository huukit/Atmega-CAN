#ifndef LIBCANBRIDGE_H
#define LIBCANBRIDGE_H

#include "libcanbridge_global.h"

#include <QString>

class LIBCANBRIDGESHARED_EXPORT LibCanBridge
{

public:
    LibCanBridge();
    QString getLibraryVersionString();
};

#endif // LIBCANBRIDGE_H
