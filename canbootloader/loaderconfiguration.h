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
 * Can bootloader configuration. (license: GPLv2 or LGPLv2.1)
 * \version 0.1.0
 * \author Tuomas Huuki tuomas.huuki@proximia.fi
 * \copyright 2014 Tuomas Huuki / proximia.fi
 * \see http://proximia.fi
 */

/** \defgroup canbootloader_config <loaderconfiguration.h> Bootloader configuration options.
 * 
 * This file includes all the configurable options for the bootloader.
 * \note Please have a look at the actual file for the documentation of configurable options.
 */

#include <avr/io.h>

#ifndef LOADERCONFIGURATION_H_
#define LOADERCONFIGURATION_H_

/** \ingroup canbootloader_config
 * \name CAN bus speed.
 * \brief Determines the speed for the can bus.
 * 
 * This options determines the speed for the CAN bus.
 * For autobauding use the definition CAN_AUTOBAUD or 0.
 */
#define CAN_BAUDRATE	250
 
/** \ingroup canbootloader_config
 * \name CAN transciever enable.
 * \brief CAN transciever hardware enable control.
 *
 * If your CAN transciever has enable/disable functionality that can be controlled by GPIO, use
 * these defines to set the correct port and IO for that. This will enable the transciever only after init
 * has been completed succesfully. If you use autobauding, the transciever will be enable immediately after
 * startup.
*/
/**\{*/
#define CANRS_PORT		PORTD
#define CANRS_DDR		DDRD
#define CANRS_PIN		PIND0
/**\}*/

/** \ingroup canbootloader_config
 * \name CAN chip hardware config.
 * \brief Pin definitions for the on chip can hardware.
 *
 * Set these per processor. Default is AtMega64C1 and equivalent.
 */
/**\{*/
#define CAN_PORT_IN     PINC
#define CAN_PORT_DIR    DDRC
#define CAN_PORT_OUT    PORTC
#define CAN_INPUT_PIN   2
#define CAN_OUTPUT_PIN  3
/**\}*/

/** \ingroup canbootloader_config
 * \name CAN chip MOB config.
 * \brief Message object buffer definitions.
 *
 * Set these per processor. Default is AtMega64C1 and equivalent. The number of TX buffers is
 * determined by subtracting the amount of RX buffers from the total amount.
 */
/**\{*/
#define NB_MOB			6
#define NB_RX_MOB		4
/**\}*/

/** \ingroup canbootloader_config
 * \name Can based debug message filtering.
 * \brief Filter CAN based debug messages (ID 0x80)
 *
 * Keep this define to use a software filter to filter out CAN debug messages used by the full 
 * protocol. Delete this in your own implementation or if you wish to receive all messages.
 */
#define FILTER_CAN_DEBUG

/** 
 * \ingroup canbootloader_config
 * \name Version information for bootloader.
 * \brief Bootloader version information.
 *
 * Change these to modify your bootloader version number that is reported in the response
 * message from the node when a session open is requested.
 *
 * \note Remember to update the documentation version also when incrementing this.
 */
/**\{*/
#define BL_VERSION_MAJOR 0 /**< Bootloader version major. */
#define BL_VERSION_MINOR 1 /**< Bootloader version minor. */
#define BL_VERSION_BUILD 0 /**< Bootloader build version. */
/**\}*/

/** 
 * \ingroup canbootloader_config
 * \name Node type definition (node identification byte).
 *
 * Change this to identify the module type you are about to flash. Can be any uint8_t value.
 * This will also be reported in the response message from the node.
 * 
 * \todo Enum?
 */
#define NODETYPEDEF		0x00 /**< Node type. */

/**
 * \ingroup canbootloader_config
 * \name Calculated number of flash pages.
 * 
 * Calculated number of flash pages calculated by dividing the flash end by the amount of bytes in a page.
 * 
 * \code 
 * #include <avr/io.h>
 * \endcode
 * \note Processor defintion files should make this work on all Atmega devices.
 */
#define SPM_MAXPAGES	(FLASHEND / SPM_PAGESIZE)

/** 
 * \ingroup canbootloader_config
 * \name EEPROM parameter placement.
 * 
 * Change these to decide where to place the node ID and bootloader activation byte in the EEPROM memory.
 */
/**\{*/
#define EEPROM_BOOTLOADER_ENABLE_ADDRESS		(uint8_t *)(0)
#define EEPROM_BOOTLOADER_ENABLE_ADDRESS_LEN	1
#define EEPROM_NODEID_ADDRESS					(uint8_t *)(EEPROM_BOOTLOADER_ENABLE_ADDRESS + EEPROM_BOOTLOADER_ENABLE_ADDRESS_LEN)
#define EEPROM_NODEID_ADDRESS_LEN				1
/**\}*/

#endif /* LOADERCONFIGURATION_H_ */