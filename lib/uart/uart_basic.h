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
 
#ifndef USART_LIBRARY_AVR
#define USART_LIBRARY_AVR

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdint.h>

#ifndef __AVR_ATmega64C1__
	#error This library does not support your processor
#endif

#define UART_BAUD 9600

uint8_t uart_init();
void uart_send(uint8_t byte);
void uart_sendline(uint8_t * buffer);
uint8_t uart_receive(uint8_t *byte);


#endif //USART_LIBRARY_AVR