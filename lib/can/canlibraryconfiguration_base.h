#ifndef _CANLIBRARYCONFIGURATION_H_
#define _CANLIBRARYCONFIGURATION_H_
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
 * Can library configuration. (license: GPLv2 or LGPLv2.1)
 * \author Tuomas Huuki tuomas.huuki@proximia.fi
 * \copyright 2014 Tuomas Huuki / proximia.fi
 * \see http://proximia.fi
 */

/** \defgroup canlibrary_config <canlibraryconfiguration.h> Can library configuration options.
 * 
 * This file includes all the configurable options for the CAN library.
 * If you have a centralized configuration file, feel free to move these there.
 */

#include <avr/io.h>

/** \ingroup canlibrary_config 
 * \name CAN bus speed.
 * \brief Determines the speed for the can bus.
 * 
 * This options determines the speed for the CAN bus.
 * For autobauding use the definition CAN_AUTOBAUD or 0.
 */
#define CAN_BAUDRATE	250
 
/** \ingroup canlibrary_config 
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

/** \ingroup canlibrary_config 
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

/** \ingroup canlibrary_config 
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

/** \ingroup canlibrary_config 
 * \name Can based debug message filtering.
 * \brief Filter CAN based debug messages (ID 0x80)
 *
 * Keep this define to use a software filter to filter out CAN debug messages used by the full 
 * protocol. Delete this in your own implementation or if you wish to receive all messages.
 */
#define FILTER_CAN_DEBUG

/** \ingroup canlibrary_config 
 * \name Interrupt handling behavior
 * \brief Use nested interrupts.
 *
 * In some cases where nested interrupts are required this switch enables the functionality. Note,
 * that will still require a few clock cycles to get the interrupts back on as special functionality
 * has to be implemented as the processor does not automatically acknowledge can interrupts.
 */
//#define CANLIB_NESTED_INTERRUPTS

#endif //_CANLIBRARYCONFIGURATION_H_