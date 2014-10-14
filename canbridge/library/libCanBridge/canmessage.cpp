#include "canmessage.h"

CanMessage::CanMessage()
{
    len = 0;
    id = 0;
    for(int i = 0; i < maxFrameDataLen; i++)
        data[i] = 0;
}

CanMessage::CanMessage(CanMessage *msg){
    recTime = msg->recTime;
    recTimeDriverMs = msg->recTimeDriverMs;
    recTimeDriverUs = msg->recTimeDriverUs;

    id = msg->id;
    len = msg->len;

    for(int i = 0; i < len && i < maxFrameDataLen; i++)
        data[i] = msg->data[i];
}

void CanMessage::insertData(uint32_t rTimeDriverMs, uint16_t rTimeDriverUs, uint32_t mid, uint8_t mrtr, uint8_t mlen, uint8_t *mdata){
    time(&recTime);
    recTimeDriverMs = rTimeDriverMs;
    recTimeDriverUs = rTimeDriverUs;
    id = mid;
    rtr = mrtr;
    len = mlen;
    for(int i = 0; i < len && i < maxFrameDataLen; i++)
        data[i] = mdata[i];
}

CanMessage::~CanMessage(){

}
