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


#ifndef BOARD_H_
#define BOARD_H_

#define DLED1 PORTC4 // Blue
#define DLED2 PORTC5 // Red
#define DLEDPORT PORTC
#define DLEDPORTDDR DDRC

inline void board_init(){
	// Init diagleds.
	DLEDPORTDDR |= _BV(DLED1) | _BV(DLED2);

}

#endif /* BOARD_H_ */