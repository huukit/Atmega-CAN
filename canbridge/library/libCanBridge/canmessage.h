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
        static const uint32_t bufLen = 128;
        char buffer [bufLen];
        asctime_s(buffer, bufLen, &timeinfo);
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
