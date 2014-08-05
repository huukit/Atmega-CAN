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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 * </copyright>
 */

/* $Id$ */

/** \file
 * Bootloader protocol message definitions. (license: GPLv2 or LGPLv2.1)
 * \author Tuomas Huuki tuomas.huuki@proximia.fi
 * \copyright 2014 Tuomas Huuki / proximia.fi
 * \see http://proximia.fi
 */

/** \defgroup canbootloader_protocol <messagedefinitions.h> Bootloader messaging protocol definitions.
 * 
 * This file includes all the definions for the protocol that is used to communicate with the bootloader.
 * \par Protocol introduction.
 * 	The communication protocol is based on CANOpen identifiers, but is not as complex as the original canopen implementation.
 *	This means that there can be up to 127 nodes that can be flashed on the same bus.
 *	The first byte of the data of the CAN frame is always the action byte. This tells the node what the network master
 *	is requesting or wanting.
 * 
 * \par Protocol usage.
 *	Usage is simple. The 11-bit CAN message identifier contains the message type and node id number. Node id numbers have to be unique.
 *	The first byte of the actual data is the command byte. Command bytes define the action that has to be taken. All nodes 
 *  must reply to the master with the same commandbyte that the master has sent.
 *	A simple example for getting the bootloader and chip version would be as follows (this is what a master would send):
 * \code
 *  // master.cpp
 *  CanMessage m;
 *  m.setId(MTYPESDOMASTER | getModuleId());
 *  m.getData(0) = MACTYPESENDBOOTLOADERVERSION;
 *  m.setDataLen(1);
 *  sendCanMessage(&m);
 * \endcode
 * The node will reply with the current version and chip id in a response message:
 * \code
 *  // CanBootloader.c
 *  m.id = MTYPESDOSLAVE| currentModuleId;
 *  m.data[0] = MACTYPESENDBOOTLOADERVERSION;
 *	m.data[1] = BL_VERSION_MAJOR;
 *	m.data[2] = BL_VERSION_MINOR;
 *	m.data[3] = BL_VERSION_BUILD;
 *	m.data[4] = boot_signature_byte_get(0x00);
 *	m.data[5] = boot_signature_byte_get(0x02);
 *	m.data[6] = boot_signature_byte_get(0x04);
 *	m.data[7] = NODETYPEDEF; 
 *	m.datalen = 8;
 *  cansend(&m);
 * \endcode
 *
 * \note See the \ref index "main documentation" page for protocol usage.
 */

#ifndef MESSAGEDEFINITIONS_H_
#define MESSAGEDEFINITIONS_H_

/** Mask for separating message TYPE and ID from CAN ID. */
#define NODE_ADDRESS_MASK 0x007F

/** Command byte position. */
#define MPOSCOMMANDBYTE 0x00 /**< Position of the command byte */
/** Error desctiption byte position. */
#define MPOSERRORDESCBYTE 0x01 /**< Position of the errpr byte */

/** \ingroup canbootloader_protocol
 * \name Message types for protocol.
 *
 * These define the two message types used by the CAN-bootloader to communicate. 
 * 
*/
/**\{*/
#define MTYPESDOSLAVE 0x0580 /**< Communication from slave(node) -> master(NMT). */
#define MTYPESDOMASTER 0x0600 /**< Communication from master(NMT) -> slave(node). */
/**\}*/

/** \ingroup canbootloader_protocol 
 * \name Message command types.
 * \code
 * data[0] is always the command.
 *
 * MACTYPERESETNODE 0x02 
 * Data length: 1.
 * Response: N/A
 *
 * MACTYPESENDBOOTLOADERVERSION 0x50 
 * Data length 1.
 * Response: 8 bytes
 * data[1] = BL_VERSION_MAJOR;
 * data[2] = BL_VERSION_MINOR;
 * data[3] = BL_VERSION_BUILD;
 * data[4] = boot_signature_byte_get(0x00);
 * data[5] = boot_signature_byte_get(0x02);
 * data[6] = boot_signature_byte_get(0x04);
 * data[7] = NODETYPEDEF;
 *
 * MACTYPEREADEEPROMBYTE 0x55 
 * Data length 3.
 * data[1] = Address to read (MSB)
 * data[2] = Address to read (LSB)
 * Response: 4 Bytes.
 * data[1] = Address read (MSB)
 * data[2] = Address read (LSB)
 * data[3] = Data from address.
 * 
 * MACTYPEWRITEEEPROMBYTE 0x56 
 * Data length 4.
 * data[1] = Address to write (MSB)
 * data[2] = Address to write (LSB)
 * data[3] = Data from address.
 * Response: 4 Bytes.
 * data[1] = Address written (MSB)
 * data[2] = Address written (LSB)
 * data[3] = Data from address.
 *
 * MACTYPEREADFLASHBYTE 0x5A 
 * Same as MACTYPEREADEEPROM
 *
 * MACTYPEREADBYTEFROMFLASHBUFFER 0x5D 
 * Same as MACTYPEREADEEPROM
 *
 * MACTYPEWRITEBYTETOFLASHBUFFER 0x5E
 * Same as MACTYPEWRITEEEPROM
 * 
 * MACTYPESELECTFLASHPAGE 0x5B 
 * Data length 2.
 * data[1] = Page to select
 * Response: 2 bytes.
 * data[1] = Selected page.
 *
 * MACTYPEWRITEFLASHPAGE 0x5C 
 * Data length 1.
 * Response: 2 bytes
 * \endcode
 */
/**\{*/
#define MACTYPERESETNODE 0x02 /**< Reset node. */

#define MACTYPESENDBOOTLOADERVERSION 0x50 /**< Request/Send bootloader version and ID information */
 
#define MACTYPEREADEEPROMBYTE 0x55 /**< Write byte to EEPROM. */
#define MACTYPEWRITEEEPROMBYTE 0x56 /**< Read byte from EEPROM. */

#define MACTYPEREADFLASHBYTE 0x5A /**< Read byte from program memory. */
#define MACTYPESELECTFLASHPAGE 0x5B /**< Select flash page. */
#define MACTYPEWRITEFLASHPAGE 0x5C /**< Write flash page. */
#define MACTYPEREADBYTEFROMFLASHBUFFER 0x5D /**< Read byte from flash page buffer. */
#define MACTYPEWRITEBYTETOFLASHBUFFER 0x5E /**< Write byte to flash page buffer. */
/**\}*/

/** \ingroup canbootloader_protocol 
 * \name Error codes and responses.
 * 
 * 0xF0 is command error byte. Second byte is error code.
 * \note See error code definitions (second data byte).
*/
/**\{*/
#define MACTYPESENDERROR 0xF0 /**< Send error */
#define ERRORBOOTLOADERSESSIONNOTOPEN 0x50 /**< Session not open (memory access requested without session). */
#define ERRORBOOTLOADERINVALIDDATA 0x51 /**< Invalid data. */
/**\}*/
#endif /* MESSAGEDEFINITIONS_H_ */