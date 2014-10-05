/*
 * <copyright>
 * Copyright (c) 2014: Tuomas Huuki / Proximia, http://proximia.fi/
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Lesser GNU General Public License for more details.
 *
 * You should have received a copy of the (Lesser) GNU General Public License
 * along with this program. If not, see <?http://www.gnu.org/licenses/>.
 * </copyright>
 */

/* $Id$ */

/** \file 
 * CAN library for AtMega64C1. (license: GPLv2 or LGPLv2.1)
 * \version 0.1
 * \author Tuomas Huuki tuomas.huuki@proximia.fi
 * \copyright 2014 Tuomas Huuki / proximia.fi
 * \see http://proximia.fi
 */
 
 /** \defgroup atmega_library <can_library.h> Main can library file for AtMega processors.
 * \todo Proper error handling and error statistics.
 * \todo Extended identifiers.
 * \todo Support over 8 MOB's
 * 
 * This is a library for AtMega processors with CAN support. It tries to utilize some of
 * the original driver supplied by Atmel and adds a configurable reception buffer implemented
 * as an ring buffer. The ring buffer is made thread safe by using the <util/atomic.h> function.
 *
 * I started work on this for the bootloader but the plan is to separate this as it's own library. 
 * There does not seem to be to many CAN library implementations around there for atmega processors.
 *
 * The library has been tested successfully with AtMega64C1 processor and can handle speeds up to 1Mbit/s.
 *
 */

#ifndef CAN_LIBRARY_AVR_
#define CAN_LIBRARY_AVR_

#include "can_drv.h"

/** \ingroup atmega_canlibrary
 * \def BOOTLOADER
 * \name Configuration file include.
 * 
 * Include your configuration file here. It must contain the following options:
 * \code
 * 
 * #define CAN_BAUDRATE	250
 * 
 * #define CANRS_PORT		PORTD
 * #define CANRS_DDR		DDRD
 * #define CANRS_PIN		PIND0
 * 
 * #define CAN_PORT_IN     PINC
 * #define CAN_PORT_DIR    DDRC
 * #define CAN_PORT_OUT    PORTC
 * #define CAN_INPUT_PIN   2
 * #define CAN_OUTPUT_PIN  3
 * 
 * #define NB_MOB			6
 * #define NB_RX_MOB		4
 * \endcode
 *
 * See the attached canlibraryconfiguration.h for an example.
 * For the sake of keeping the bootloader configurin as simple as possible, I have included it here.
 * Feel free to remove this if you see it necessary for some reason.
 */
#ifdef BOOTLOADER
	#include "loaderconfiguration.h"
#else
	#include "canlibraryconfiguration.h"
	#ifndef _CANLIBRARYCONFIGURATION_H_
		#error You need to #include the "configuration.h" file to set port and speed options.
	#endif
#endif

#define CAN_BUFFEREMPTY -1

/** \ingroup atmega_canlibrary
 * \name CAN debug message filtering.
 * \todo Implement proper filtering.
 *
 * Filter CAN debugging messages. You can safely ignore this if you do not need it. It just utilizes
 * a simple software filter to block out debug messages for my own protocol.
 */
/**\{*/
#ifdef FILTER_CAN_DEBUG
	#define MTYPESYNCEMER 0x0080
	#define NODE_ADDRESS_MASK 0x007F
#endif
/**\}*/

/** \ingroup atmega_canlibrary
 * \name CAN message structure.
 * 
 * The main structure for a can message.
 */
typedef struct _canMessage{
	uint32_t cantime;	/**< CAN message timestamp. */
	uint32_t mesid;		/**< CAN message ID. */
	uint8_t rtr;		/**< CAN message RTR flag. */
	uint8_t length;		/**< CAN message data length. */
	uint8_t data[8];	/**< CAN message data buffer. */
} canMessage;

/** \ingroup atmega_canlibrary
 * \name Transmission object buffer allocation count and check.
 *
 * Calculate the number of transmission buffers from the total amount.
 * Define the first transmission object buffer.
 */
/**\{*/
#define NB_TX_MOB (NB_MOB - NB_RX_MOB)

#if (NB_TX_MOB < 1)
	#error You will need at least 1 object buffer to be able to transmit messages.
#endif
#if (NB_MOB > 8)
	#error This version of canlib does not support more than 8 MOBs.
#endif

#define FIRST_TX_MOB NB_RX_MOB
/**\}*/

/** \ingroup atmega_canlibrary
 * \brief Initialize the bus and activate hardware.
 * \retval 0 Init/autobaud failure.
 * \retval 1 Init/autobaud success.
 *
 * Initializes the driver, speed and also activates the transciever if the option has been configured.
 * \code
 * #define CAN_BAUDRATE	250 // Use 0 for autobaud.
 * \endcode
 */
uint8_t can_init();

/** \ingroup atmega_canlibrary
 * \brief Disable driver & hardware.
 *
 * Disabled the driver and the transciever if the option has been configured.
 */
void can_close();

/** \ingroup atmega_canlibrary
 * \brief Sends a can message.
 * \param m Pointer to a canMessage object that has been configured.
 * \retval 0 Message NOT sent.
 * \retval 1 Message sent.
 * 
 * Sends a canmessage with the configured id and data. Usage example:
 * \code
 * canMessage m;
 * m.mesid = MTYPESDOSLAVE | nodeid;
 * m.data[1] = BL_VERSION_MAJOR;
 * m.data[2] = BL_VERSION_MINOR;
 * m.data[3] = BL_VERSION_BUILD;
 * m.data[4] = boot_signature_byte_get(0x00);
 * m.data[5] = boot_signature_byte_get(0x02);
 * m.data[6] = boot_signature_byte_get(0x04);
 * m.data[7] = NODETYPEDEF;
 * m.length = 8;
 * // Blocking (use return value for information and resend on next loop for non blocking operation).
 * while(!can_sendMessage(&m));
 * \endcode
 */
uint8_t can_sendMessage(canMessage *m);

/** \ingroup atmega_canlibrary
 * \brief Gets a can message from the reception buffer.
 * \param m Pointer to message.
 * \retval -1 No message in buffer.
 * \retval n Number of messages in buffer.
 * 
 * Retrieves a message from the internal circular buffer and returns the amount 
 * of messages left in the buffer OR CAN_BUFFEREMPTY if the buffer is empty.
 * You may use either:
 * \code 
 * if(can_getMessage(&m) != CAN_BUFFEREMPTY){
 *	  // Process one message.
 * }
 * \endcode
 * or
 * \code
 * while(can_getMessage(&m) != CAN_BUFFEREMPTY){
 *	 // Process until buffer empty.
 * }
 * \endcode
 * depending on your implementation, buffer length and bus speed. 
 */
int8_t can_getMessage(canMessage *m);

/** \ingroup atmega_canlibrary
 * \brief Returns the number of messages in the buffer.
 * \return n Number of messages in buffer.
 */
uint8_t can_getRxBufferLength();

/** \ingroup atmega_canlibrary
 * \name Transciever control.
 * \brief Enable/disable the transciever if supported by your hardware.
 * 
 * Define the pin in the configuration file.
 * \code
 * #define CANRS_PORT		PORTD
 * #define CANRS_DDR		DDRD
 * #define CANRS_PIN		PIND0
 * \endcode
 */
/**\{*/
void can_enableTransciever();
void can_disableTransciever();
/**\}*/
	
#endif //CAN_LIBRARY_AVR_
