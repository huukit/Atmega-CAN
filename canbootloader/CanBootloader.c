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
 * Can bootloader support. (license: GPLv2 or LGPLv2.1)
 * \author Tuomas Huuki tuomas.huuki@proximia.fi
 * \copyright 2014 Tuomas Huuki / proximia.fi
 * \see http://proximia.fi
 */

/** \defgroup canbootloader_main <canbootloader.c>: Main bootloader file.
 *
 *	This is the main file for the CAN bootloader. 
 */

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/boot.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include <stdint.h>

#include "can_library.h"
#include "loaderconfiguration.h"
#include "messagedefinitions.h"

/** \internal
 * \ingroup canbootloader_main
 *	\brief Mirror of the MCUCSR register, taken early during startup.
 *	\endinternal
*/
uint8_t mcusr __attribute__((section(".noinit")));

/** \internal
 * \ingroup canbootloader_main
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


/** \ingroup canbootloader_main
*	Temporary flash buffer
*/
uint8_t flashblock[SPM_PAGESIZE];

/** \ingroup canbootloader_main
*	Current flashpage that is used.
*/
uint8_t currentPage = 0;

/** \ingroup canbootloader_main
*	Current byte used in current page
*/
uint32_t currentPageByte = 0;

/** \ingroup canbootloader_main
*	\brief Write temporary buffer to flash.
*	\param address Page address to write to.
*	\param buf Pointer to page buffer.
*/
void bootProgramPage (uint32_t address, uint8_t *buf);

/** \ingroup canbootloader_main
*	\brief Clear the flash pagebuffer.
*/
void clearFlashBuffer();

/** \ingroup canbootloader_main
*	\brief Jump to application.
*	
*	Moves the interrupts to the application section and jumps to the application section (0x0000).
*/
void startApplication();

/** \ingroup canbootloader_main
*	\brief Main bootloader code.
*/
int main()
{
	// Disable watchddog.
	wdt_disable();
	
	// Move interrupts to Boot Flash section.
	MCUCR = _BV(IVCE);
	MCUCR = _BV(IVSEL);
		
	// Check if flash programmed.
	uint8_t flashstart = pgm_read_byte(0);
	if(flashstart != 0xFF){
		// Chceck if bootloader requested
		uint8_t bootLoaderRequest = eeprom_read_byte(EEPROM_BOOTLOADER_ENABLE_ADDRESS);
		// If there is something else than 0 written to the first part of eeprom, we can assume corruption or a request.
		if(bootLoaderRequest == 0)
			startApplication();
	}
	
	// Initialize bus and enable interrupts.
	can_init();	
	sei();
	
	// Clear local flash buffer.
	clearFlashBuffer();
		
	// Get current nodeid for message filtering. Set default if invalid id, set to 1.
	uint8_t nodeid = eeprom_read_byte(EEPROM_NODEID_ADDRESS);
	if(nodeid > NODE_ADDRESS_MASK)nodeid = 0x01;
	
	// Local variables
	uint8_t mesid = 0;
	uint16_t mtype = 0;
	uint8_t mcommand = 0;
	
	canMessage m;
	uint8_t doLoaderTask = 1;
	uint8_t sessionOpen = 0;
	
	// Loop loadertask until exit requested.
    while(doLoaderTask)
    {
		if(can_getMessage(&m) != CAN_BUFFEREMPTY){
			// Break can frame identifier in to node id and message type.
			mesid = m.mesid & NODE_ADDRESS_MASK;
			mtype = m.mesid & ~NODE_ADDRESS_MASK;
			// Message from master (our ID) & type is SDO.
   			if(mesid == nodeid && mtype == MTYPESDOMASTER){		
				mcommand = m.data[MPOSCOMMANDBYTE];
				/*
				 * Messages that require a response at any time.
				 */
				switch(mcommand){
					// Handle session and send version information.
					case MACTYPESENDBOOTLOADERVERSION:
					{
						m.mesid = MTYPESDOSLAVE | nodeid;
						m.data[1] = BL_VERSION_MAJOR;
						m.data[2] = BL_VERSION_MINOR;
						m.data[3] = BL_VERSION_BUILD;
						m.data[4] = boot_signature_byte_get(0x00);
						m.data[5] = boot_signature_byte_get(0x02);
						m.data[6] = boot_signature_byte_get(0x04);
						m.data[7] = NODETYPEDEF;
						m.length = 8;
						sessionOpen = 1;
						can_sendMessage(&m);
						break;
					}
					// Reset and jump to application.
					case MACTYPERESETNODE:
					{
						eeprom_write_byte(EEPROM_BOOTLOADER_ENABLE_ADDRESS, 0x00);
						eeprom_busy_wait();
						sessionOpen = 0;
						doLoaderTask = 0;
						break;
					}
					default:
						break;
				}
				
				/* 
				 * Messages with memory operations only handled when session open.
				 */
				if (sessionOpen){
					m.mesid = MTYPESDOSLAVE | nodeid;
					switch(mcommand){
						// Read eeprom byte.
						case MACTYPEREADEEPROMBYTE:
						{
							if(m.length != 3){ // Min length of command 
								m.data[MPOSCOMMANDBYTE] = MACTYPESENDERROR;
								m.data[MPOSERRORDESCBYTE] = ERRORBOOTLOADERINVALIDDATA;
								m.length = 2;
							}
							else{
								eeprom_busy_wait();
								uint16_t eepromaddr = ((m.data[1] << 8) | m.data[2]);
								m.data[3] = eeprom_read_byte((uint8_t *)(eepromaddr));
								m.length = 4;
							}
							can_sendMessage(&m);
							break;
						}
						// Write eeprom byte.
						case MACTYPEWRITEEEPROMBYTE:
						{
							if(m.length != 4){ // Min length of command 
								m.data[MPOSCOMMANDBYTE] = MACTYPESENDERROR;
								m.data[MPOSERRORDESCBYTE] = ERRORBOOTLOADERINVALIDDATA;
								m.length = 2;
							}
							else{
								eeprom_busy_wait();
								uint16_t eepromaddr = ((m.data[1] << 8) | m.data[2]);
								eeprom_write_byte((uint8_t *)(eepromaddr), m.data[3]);
								eeprom_busy_wait();
								m.data[3] = eeprom_read_byte((uint8_t *)(eepromaddr));
								m.length = 4;
							}
							can_sendMessage(&m);
							break;
						}
						// Read byte from flash.
						case MACTYPEREADFLASHBYTE:
						{
							if(m.length != 3){ // Min length of command 
								m.data[MPOSCOMMANDBYTE] = MACTYPESENDERROR;
								m.data[MPOSERRORDESCBYTE] = ERRORBOOTLOADERINVALIDDATA;
								m.length = 2;
							}
							else{
								uint16_t flashaddr = ((m.data[1] << 8) | m.data[2]);
								m.data[3] = pgm_read_byte(flashaddr);
								m.length = 4;
							}
							can_sendMessage(&m);
							break;
						}
						// Select flash page.
						case MACTYPESELECTFLASHPAGE:
						{
							if(m.length != 2){
								m.data[MPOSCOMMANDBYTE] = MACTYPESENDERROR;
								m.data[MPOSERRORDESCBYTE] = ERRORBOOTLOADERINVALIDDATA;
								m.length = 2;
							}
							else{
								clearFlashBuffer();
								currentPage = m.data[1];
								m.length = 2;
							}
							can_sendMessage(&m);
							break;
							
						}
						// Write flash page.
						case MACTYPEWRITEFLASHPAGE:
						{
							if(m.length != 1){
								m.data[MPOSCOMMANDBYTE] = MACTYPESENDERROR;
								m.data[MPOSERRORDESCBYTE] = ERRORBOOTLOADERINVALIDDATA;
								m.length = 2;
							}
							else{
								// Write 0xFF to eeprom when flashing to enable bootloader in case of flashing problems.
								eeprom_write_byte(EEPROM_BOOTLOADER_ENABLE_ADDRESS, 0xFF);
								eeprom_busy_wait();
								bootProgramPage(currentPage * SPM_PAGESIZE, flashblock);
							}
							can_sendMessage(&m);
							break;
						}
						// Write byte to temporary flash buffer.
						case MACTYPEWRITEBYTETOFLASHBUFFER:
						{
							if(m.length != 4){
								m.data[MPOSCOMMANDBYTE] = MACTYPESENDERROR;
								m.data[MPOSERRORDESCBYTE] = ERRORBOOTLOADERINVALIDDATA;
								m.length = 2;
							}
							else{
								uint16_t blockaddr = ((m.data[1] << 8) | m.data[2]);
								if(blockaddr < SPM_PAGESIZE){
									flashblock[blockaddr] = m.data[3];
								}
								else{
									m.data[MPOSCOMMANDBYTE] = MACTYPESENDERROR;
									m.data[1] = ERRORBOOTLOADERINVALIDDATA;
									m.length = 2;
								}
							}
							can_sendMessage(&m);
							break;
						}
						// Read byte from temporary flash buffer.
						case MACTYPEREADBYTEFROMFLASHBUFFER:
						{
							if(m.length != 3){
								m.data[MPOSCOMMANDBYTE] = MACTYPESENDERROR;
								m.data[MPOSERRORDESCBYTE] = ERRORBOOTLOADERINVALIDDATA;
								m.length = 2;
							}
							else{
								uint16_t blockaddr = ((m.data[1] << 8) | m.data[2]);
								if(blockaddr < SPM_PAGESIZE){
									m.data[3] = flashblock[blockaddr];
									m.length = 4;
								}
								else{
									m.data[MPOSCOMMANDBYTE] = MACTYPESENDERROR;
									m.data[MPOSERRORDESCBYTE] = ERRORBOOTLOADERINVALIDDATA;
									m.length = 2;
								}
							}
							can_sendMessage(&m);
							break;
						}
						default:
							break;
					}
				}
				// If memory operations are requested without opening the session, send error.
				else if(doLoaderTask){
					m.mesid = MTYPESDOSLAVE | nodeid;
					m.data[MPOSCOMMANDBYTE] = MACTYPESENDERROR;
					m.data[MPOSERRORDESCBYTE] = ERRORBOOTLOADERSESSIONNOTOPEN;
					m.length = 2;
					can_sendMessage(&m);
				}
			}
		}
    }
	// On loop exit, start the main application.
	startApplication();
}

void startApplication(){
		// Dsable interrupts.
		cli();
		
		// Move interrupts back to Application Flash section
		MCUCR = _BV(IVCE);
		MCUCR = 0;
		
		// Jump to main program start.
		asm("jmp 0000");
}

void clearFlashBuffer(){
		// Initialize flashbuffer.
		for(int i = 0; i < SPM_PAGESIZE; i++)
			flashblock[i] = 0xFF;
		
		currentPage = 0;
		currentPageByte = 0;
}

 void bootProgramPage (uint32_t address, uint8_t *buf)
 {
	 uint16_t i;
	 uint8_t sreg;

	 // Disable interrupts.

	 sreg = SREG;
	 cli();
	 
	 eeprom_busy_wait ();

	 boot_page_erase (address);
	 boot_spm_busy_wait ();      // Wait until the memory is erased.

	 for (i = 0; i < SPM_PAGESIZE; i += 2)
	 {
		 // Set up little-endian word.

		 uint16_t w = *buf++;
		 w += (*buf++) << 8;
		 
		 boot_page_fill (address + i, w);
	 }

	 boot_page_write (address);     // Store buffer in flash page.
	 boot_spm_busy_wait();       // Wait until the memory is written.
	 
	 // Enable read after writing.
	 boot_rww_enable_safe();
	 
	 // Re-enable interrupts (if they were ever enabled).

	 SREG = sreg;
 }