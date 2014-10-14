// <copyright file="noauth.cs" company="Proximia">
// Copyright (c) 2014 All Right Reserved, http://proximia.fi/
//
// All rights reserved.
//
// THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// </copyright>
//
// <author>Tuomas Huuki</author>
// <email>tuomash@proximia.fi</email>
// <date></date>
// <summary></summary>

#ifndef CANMESSAGE_H
#define CANMESSAGE_H

#include <stdint.h>
#include <time.h>
#include <string>

class CanMessage
{

public:
    static const uint8_t maxFrameDataLen = 8;

    CanMessage();
    CanMessage(CanMessage *msg);
    ~CanMessage();

    void insertData(uint32_t rTimeDriverMs, uint16_t rTimeDriverUs, uint32_t mid, uint8_t mrtr, uint8_t mlen, uint8_t * mdata);

    const std::string getTime() const{
        tm timeinfo;
        localtime_s(&timeinfo, &recTime);
        char buffer [128];
        asctime_s(buffer ,&timeinfo);
        std::string timenow(buffer);
        return timenow;
    }

    const uint32_t getId() const{
        return id;
    }

    const uint8_t getLen() const{
        return len;
    }

    void setData(const uint8_t * candata, uint8_t dlen){
        if(dlen > 8)dlen = 8;
        len = dlen;
        for(uint32_t i = 0; i < dlen; i++)
            data[i] = candata[i];
    }

    const uint8_t * getData() const{
        return data;
    }
    const uint32_t getRevTimeDriverMs() const{
        return recTimeDriverMs;
    }

    const uint8_t getRtr() const{
        return rtr;
    }

    const time_t getTimeRaw() const{
        return recTime;
    }

private:
    // Timestamp computer
    time_t recTime;

    // Timestamp driver/local
    uint32_t recTimeDriverMs;
    uint16_t recTimeDriverUs;

    // Data
    uint32_t id;
    uint8_t len;
    uint8_t rtr;
    uint8_t data[maxFrameDataLen];
};

#endif // CANMESSAGE_H
