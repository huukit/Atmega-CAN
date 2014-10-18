/** \file 
 * CAN library for AtMega64C1. (license: GPLv2 or LGPLv2.1)
 * \version 0.1
 * \author Tuomas Huuki tuomas.huuki@proximia.fi
 * \copyright 2014 Tuomas Huuki / proximia.fi
 * \see http://proximia.fi
 */

#include "can_library.h"
#include <util/atomic.h>

volatile unsigned char msg_received = 0;
volatile uint32_t cantimecounter = 0;

/** \internal 
 * \ingroup atmega_canlibrary
 * \name Can driver buffer configuration.
 */
/**\{*/
#define RING_SIZE 32
volatile uint8_t ring_head;
volatile uint8_t ring_tail;
volatile canMessage ring_data[RING_SIZE];
/**\}*/

uint8_t can_getRxBufferLength(){
	return msg_received;
}

/** \internal 
 * \ingroup atmega_canlibrary
 * \brief allocates a message from the buffer, returns 0 if buffer full.
 */
int canbuf_add(canMessage **m) {
	uint8_t next_head = (ring_head + 1) % RING_SIZE;
	if (next_head != ring_tail) {
		*m = (canMessage *)&ring_data[ring_head];
		ring_head = next_head;
		return 1;
	} else {
		return 0;
	}
}

/** \internal 
 * \ingroup atmega_canlibrary
 * \brief frees a message from the buffer, returns 0 if empty.
 */
int canbuf_remove(canMessage *m){
	if (ring_head != ring_tail) {
		m->mesid = ring_data[ring_tail].mesid;
		m->length = ring_data[ring_tail].length;
		m->cantime = ring_data[ring_tail].cantime;
		m->rtr = ring_data[ring_tail].rtr;
		for(int i = 0; i < m->length; i++)
			m->data[i] = ring_data[ring_tail].data[i];
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
			ring_tail = (ring_tail + 1) % RING_SIZE;
			msg_received--;
		}
		return 1;
	} else {
		return 0;
	}
}

// See if we have any free tx-buffers.
int8_t can_hasFreeTxBuffer(){
	for (uint32_t i = FIRST_TX_MOB; i < NB_MOB; i++){
		Can_set_mob(i);
		if(can_get_mob_status() == MOB_DISABLE){
			return i;
		}
	}	
	return -1;
}

// Enable the CAN transciever hardware
void can_enableTransciever(){
	CANRS_DDR |= _BV(CANRS_PIN);
	CANRS_PORT &= ~_BV(CANRS_PIN);	
}

// Disable the CAN transciever hardware
void can_disableTransciever(){
	CANRS_DDR |= _BV(CANRS_PIN);
	CANRS_PORT |= _BV(CANRS_PIN);
}

// Init hardware
uint8_t can_init(){
	// Set necessary I/O
	CAN_PORT_DIR &= ~_BV(CAN_INPUT_PIN );
	CAN_PORT_DIR &= ~_BV(CAN_OUTPUT_PIN);
	CAN_PORT_OUT |=  _BV(CAN_INPUT_PIN );
	CAN_PORT_OUT |=  _BV(CAN_OUTPUT_PIN);
	
	// Disable transciever. If autobauding is requested enable before autobauding.
	can_disableTransciever();
	  
	uint8_t baudratecomplete = 0;
	if(CAN_BAUDRATE == CAN_AUTOBAUD){
		can_enableTransciever();
		baudratecomplete = can_auto_baudrate(0);
	}
	else{
		baudratecomplete = can_fixed_baudrate(0);
	}
	
	// If failure.	
	if(baudratecomplete == 0)
		return baudratecomplete;
	
	// Clear all driver buffers.
	can_clear_all_mob();
	
	// Initialize reception buffers.
	for(uint32_t i = 0; i < NB_RX_MOB; i++){
		Can_set_mob(i);
		Can_config_rx();
		Can_clear_status_mob();
	}
	
	/* Enable interrupts. 
		NOTE: The AtMega documentation is a little odd here. ENIT is supposed to enable all interrupts
		so I cannot figure out what the other bits do because they do not do anything on their own..
	*/
	CANGIE = _BV(ENIT) | _BV(ENRX) | _BV(ENTX) | _BV(ENOVRT) | _BV(BOFFIT);
	
	/* Enable message object buffers.
		NOTE: This is processor dependent so add defines when tested on other platforms.
	*/
	#if defined (__AVR_ATmega64C1__) | (__AVR_ATmega32C1__)
		CANIE1 = 0x00;
		CANIE2 = _BV(IEMOB0) | _BV(IEMOB1) | _BV(IEMOB2) | _BV(IEMOB3) | _BV(IEMOB4) | _BV(IEMOB5);   
	#else 
		#error You must define the MOB registers in can_library.c (can_init) for your processor.
	#endif

	// Enable chip hardware.
	Can_enable();           
  
	// Enable transceiver hardware if necessary.
	if(CAN_BAUDRATE != CAN_AUTOBAUD)
		can_enableTransciever();
  
	return baudratecomplete;
}

void can_close(){
	// Disable transciever.
	can_disableTransciever();
	
	// Disable hardware.
	Can_disable();
	
	// Disable all interrupts.
	CANGIE = 0x00;
	CANIE1 = 0x00;
	CANIE2 = 0x00;
	
	// Clear all data
	can_clear_all_mob();
}

// Send a message pointed by m.
uint8_t can_sendMessage(canMessage *m){
	int8_t bufferfound = 0;

	bufferfound = can_hasFreeTxBuffer();
	
	// Buffer found. Clear and init with m.
	if(bufferfound >= 0){
		Can_set_mob(bufferfound);
		
		CANCDMOB = 0;
		Can_clear_status_mob();
		
		Can_set_std_id(m->mesid); 
		if(m->rtr) 
			Can_set_rtr();
 
		Can_set_dlc(m->length);		

		for (uint32_t i= 0; i < (m->length); i++)	
			CANMSG = m->data[i];
	    
		// Configure MOB as TX, this will generate an interrupt when sent.
		Can_config_tx();		
		return 1;	
	}
	// No free buffers.
	else{
		return 0;
	}
}

// Return a message from the internal buffer.
int8_t can_getMessage(canMessage *m){
	if(!msg_received)return CAN_BUFFEREMPTY;
	uint8_t status = 0;
	
	status = canbuf_remove(m);
	if (status){
		status = can_getRxBufferLength();
	}
  
  return status;
}

/** \internal 
 * \ingroup atmega_canlibrary
 * \brief Can timer interrupt.
 */
SIGNAL(CAN_TOVF_vect)
{
	// Per documentation, visiting the interrupt clears the bit.
	cantimecounter++;
}

/** \internal 
 * \ingroup atmega_canlibrary
 * \brief Can driver interrupt.
 *
 * Main can interrupt. Handles RX, TX and error situations. TODO: Proper error handling.
 */
SIGNAL(CAN_INT_vect){
	uint8_t cangit = CANGIT;
	CANGIT |= (_BV(BOFFIT) | _BV(SERG) | _BV(CERG) | _BV(FERG) | _BV(AERG));

	// If we need nested interrupts, disable MOb interrupts or this will end in an death loop for some reason.	
	#ifdef CANLIB_NESTED_INTERRUPTS
		#if defined (__AVR_ATmega64C1__) | (__AVR_ATmega32C1__)
			CANIE1 = 0x00;
			CANIE2 = 0x00;
		#else
			#error You must define the MOB registers in can_library.c (can_init) for your processor.
		#endif
		sei();
	#endif
	
	uint8_t canerror = cangit & (_BV(BOFFIT) | _BV(SERG) | _BV(CERG) | _BV(FERG) | _BV(AERG));
			
	canMessage *m = 0;
	uint8_t status = 0;
	
	/* Save the page that is selected as the interrupt will change this and a transmission might be taking
		place at the same time. Restore it before exiting the interrupt handler so any function using the page
		can continue normally after the interrupt.
	*/
	uint8_t currentPage = CANPAGE;
	
	// No error, see what needs processing.
	if(!canerror){
		// RX interrupt
		// TODO: This will break if more than 8 buffers. Test with actual hardware.
		if(CANSIT2 & ~(0xFF << NB_RX_MOB) /*|| (CANSIT1 & ~(0xFF << NB_RX_MOB - 8))*/){
			// Loop through the buffers and see which one needs handling.
			for(uint32_t i = 0; i < NB_RX_MOB; i++){
				// We can look at the CANSIT2 register to see if a MOB has caused interrupt.
				// TODO: This will break if more than 8 buffers. Test with actual hardware.
				if(CANSIT2 & (1 << i) /*|| ((CANSIT & (1 << i)) && i > 7)*/){
					Can_set_mob(i);
					// NOTE: If DLC changes, DLCW will be active. This is a warning, not an error.
					if(can_get_mob_status() == MOB_RX_COMPLETED || can_get_mob_status() == MOB_RX_COMPLETED_DLCW){
						uint16_t id;
						Can_get_std_id(id);
// Software message filtering, feel free to ignore.				
#ifdef FILTER_CAN_DEBUG 
						if((id & (~NODE_ADDRESS_MASK)) != MTYPESYNCEMER){
#endif							
							// Allocate buffer and save data from driver.
							status = canbuf_add(&m);
							if(status != 0 && m != 0){
								m->mesid = id;
								m->rtr = Can_get_rtr();
								m->length = Can_get_dlc();
								can_get_data(m->data);
								m->cantime = cantimecounter;
								msg_received++;
							}
#ifdef FILTER_CAN_DEBUG													
						}
#endif						
					}
					else{
						// TODO: Handle error if any.
					}	
					// Set to rx and clear status.		
					Can_config_rx();
					Can_clear_status_mob();
				}
			}	
		}
		// TX interrupt
		else{
			for (uint32_t i = NB_RX_MOB; i < NB_MOB; i++){
				if(CANSIT2 & (1 << i) /*|| ((CANSIT & (1 << i)) && i > 7)*/){
					Can_set_mob(i);
					if (can_get_mob_status() == MOB_TX_COMPLETED)
					{
						// TODO: Log statistics, consider implementing a handler to the user app.
					}
					else{
						// TODO: Handle error if any.
					}
					// Clear status and set to disable.
					Can_clear_status_mob();
					DISABLE_MOB;
				}
			}
		}
	}
	// Errors, log or reset if busoff.
	else{
		if(cangit & _BV(BOFFIT)){
			can_init();
		}
		else{
			// Log other errors.
		}
	}

	CANPAGE = currentPage;
	#ifdef CANLIB_NESTED_INTERRUPTS
		#if defined (__AVR_ATmega64C1__) | (__AVR_ATmega32C1__)
			CANIE1 = 0x00;
			CANIE2 = _BV(IEMOB0) | _BV(IEMOB1) | _BV(IEMOB2) | _BV(IEMOB3) | _BV(IEMOB4) | _BV(IEMOB5);
		#else
			#error You must define the MOB registers in can_library.c (can_init) for your processor.
		#endif
	#endif
}


