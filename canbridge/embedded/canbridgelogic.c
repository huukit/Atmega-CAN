#include "canbridgelogic.h"
#include "can/can_library.h"

#include "canlibraryconfiguration.h"
#include "can/can_library.h"

#include "usbconfig.h"
#include "3rdparty/usbdrv/usbdrv.h"
#include "3rdparty/usbdrv/oddebug.h"

#include "board.h"

static uint8_t currentPosition, bytesRemaining, canbuflen;
#define CANPACKETSIZE 18
static uint8_t usbRxBuffer[CANPACKETSIZE];
static uint8_t usbTxBuffer[CANPACKETSIZE];

uint8_t intServed = 0;
uint8_t txWaiting = 0;
uint8_t txSent = 0;

#define USB_COMMAND_INITBUS 1
#define USB_COMMAND_CLOSEBUS 2
#define USB_COMMAND_GETCANDATA 3
#define USB_COMMAND_SENDCANDATA 4

uint8_t initok = 0;

uchar usbFunctionSetup(uchar setupData[8]){
	usbRequest_t *rq = (void *)setupData;   // cast to structured data for parsing
	switch(rq->bRequest){
		case USB_COMMAND_INITBUS:
			uart_sendline("Opening bus.");
			can_init();
			initok = 1;
		break;
		case USB_COMMAND_CLOSEBUS:
			uart_sendline("Closing bus.");
			can_close();
			initok = 0;
		break;
		case USB_COMMAND_GETCANDATA:
			if(!initok)break;
			PORTC &= ~_BV(DLED1);
			can_getMessage((canMessage *)usbRxBuffer);
			currentPosition = 0;
			bytesRemaining = rq->wLength.word;
			return USB_NO_MSG;
		case USB_COMMAND_SENDCANDATA:
			if(!initok)break;
			PORTC &= ~_BV(DLED1);
			currentPosition = 0;
			bytesRemaining = rq->wLength.word;
			return USB_NO_MSG;
		default:
		break;
	}
	return 0; // ignore all unknown requests
}

uchar usbFunctionWrite(uchar *data, uchar len){
	if(len > bytesRemaining)
	len = bytesRemaining;
	
	bytesRemaining -= len;
	for(int i = 0; i < len; i++)
	usbTxBuffer[currentPosition++] = data[i];
	
	if(bytesRemaining == 0){
		txWaiting = 1;
		PORTC |= _BV(DLED1);
	}
	
	return bytesRemaining == 0;
}

void usbFunctionWriteOut(uchar *data, uchar len){

}

uchar usbFunctionRead(uchar *data, uchar len)
{
	if(len > bytesRemaining)
	len = bytesRemaining;
	
	bytesRemaining -= len;
	for(int i = 0; i < len; i++)
	data[i] = usbRxBuffer[currentPosition++];
	
	if(bytesRemaining == 0 && canbuflen > 0)
	canbuflen--;

	if(canbuflen == 0){
		PORTC |= _BV(DLED1);
		intServed = 1;
	}
	
	return len;
}

void bridgelogic_init(){
	odDebugInit();
	usbInit();
	
	intServed = 1;
	txWaiting = 0;
}

void bridgelogic_poll(){
		if(can_getRxBufferLength() && initok){
			if(usbInterruptIsReady() && intServed){
				intServed = 0;
				canbuflen = can_getRxBufferLength();
				usbSetInterrupt(&canbuflen, 1);
			}
		}
		if((txWaiting || txSent == 0) && initok){
			txSent = can_sendMessage((canMessage *)usbTxBuffer);
			txWaiting = 0;
		}	
}