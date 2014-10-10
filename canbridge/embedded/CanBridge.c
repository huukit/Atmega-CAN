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
 * CanBridge main . (license: GPLv2 or LGPLv2.1)
 * \author Tuomas Huuki tuomas.huuki@proximia.fi
 * \copyright 2014 Tuomas Huuki / proximia.fi
 * \see http://proximia.fi
 */

/** \defgroup canbridge_main <canbridge.c>: Main CanBridfe file.
 *
 *	This is the main file for the CanBridge hardware. 
 */

#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "canbridgelogic.h"

#include "board.h"
#include "uart/uart_basic.h"

#include "3rdparty/usbdrv/usbdrv.h"

/** \internal
 * \ingroup canbridge_main
 *	\brief Mirror of the MCUCSR register, taken early during startup.
 *	\endinternal
*/
uint8_t mcusr __attribute__((section(".noinit")));

/** \internal
 * \ingroup canbridge_main
 * \brief Handle the MCUSR register
 * \endinternal
*/
void handle_mcucsr()
__attribute__((section(".init3")))
__attribute__((naked));
void handle_mcucsr()
{
	mcusr = MCUSR;
	MCUSR = 0;
}

int main(void)
{
	wdt_disable();
	
	uart_init();
	board_init();
	
	// Switch on leds for init.
	DLEDPORT &= ~(_BV(DLED1) | _BV(DLED2));
		
	// Force reconnect.
	usbDeviceDisconnect();
	_delay_ms(500);
	usbDeviceConnect();
	bridgelogic_init();
	
	uart_sendline("B00t\n\r");
	sei();
		
	DLEDPORT |= _BV(DLED1) | _BV(DLED2);
	
    while(1)
    {
        usbPoll();
		bridgelogic_poll();
	}
}