#ifndef SIGNALTRAMPOLINE_H
#define SIGNALTRAMPOLINE_H

#include <QObject>
#include <stdint.h>

#include "libcanbridge.h"

class SignalTrampoline : public QObject{
    Q_OBJECT
public:
    friend class LibCanBridge;

    SignalTrampoline(QObject * parent = 0) :
        QObject(parent){
        intCallback = 0;
    }

public slots:
    void messageInt(uint32_t buflen){
        if(intCallback)intCallback(buflen);
    }

private:
    rxIntCallback intCallback;
};

#endif // SIGNALTRAMPOLINE_H
