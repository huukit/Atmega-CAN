#include "uart_basic.h"

uint8_t uart_init(){
	LINCR = (1 << LSWRES);
	LINBRRH = (((F_CPU/UART_BAUD)/16)-1)>>8;
	LINBRRL = (((F_CPU/UART_BAUD)/16)-1);
	LINBTR = (1 << LDISR)| (16 << LBT0);
	LINCR = (1<<LENA)|(1<<LCMD2)|(1<<LCMD1)|(1<<LCMD0);
}

void uart_send(uint8_t byte){
	LINDAT = byte;	
	while (!(LINSIR & (1 << LENTXOK)));
}

void uart_sendline(uint8_t * buffer){
	while (*buffer != 0)
		uart_send(*buffer++);
}

uint8_t uart_receive(uint8_t *byte){
	if(LINSIR & _BV(LENRXOK)){
		*byte = LINDAT;
		return 1;
	}
	return 0;
}

