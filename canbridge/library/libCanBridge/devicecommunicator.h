#ifndef DEVICECOMMUNICATOR_H
#define DEVICECOMMUNICATOR_H

#include <QObject>
#include "libusb.h"

class DeviceCommunicator : public QObject
{
    Q_OBJECT
public:
    explicit DeviceCommunicator(libusb_device_handle * handle, QObject *parent = 0);

signals:

public slots:

private:
    libusb_device_handle * usbBeviceHandle;
};

#endif // DEVICECOMMUNICATOR_H
