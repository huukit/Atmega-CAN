/*
 * <copyright>
 * Copyright (c) 2014: Tuomas Huuki / Proximia, http://proximia.fi/
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Lesser GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <?http://www.gnu.org/licenses/>.
 * </copyright>
 */

#ifndef SIGNALTRAMPOLINE_H
#define SIGNALTRAMPOLINE_H

#include <QObject>
#include <stdint.h>

#include "libcanbridge.h"

/* Just a simple helper class to use when the user application does not use QT.
    This will take the internal signal and call a callback function registered by the user.
*/

class SignalTrampoline : public QObject{
    Q_OBJECT
public:
    friend class LibCanBridge;

    SignalTrampoline(QObject * parent = 0) :
        QObject(parent){
        intCallback = 0;
    }

public slots:
    void messageInt(int buflen){
        if(intCallback)intCallback(buflen);
    }

private:
    rxIntCallback intCallback;
};

#endif // SIGNALTRAMPOLINE_H
