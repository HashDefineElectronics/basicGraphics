/*
 * display.h
 *
 *  Created on: Dec 18, 2018
 *      Author: Ronald Sousa
 */

#ifndef __DISPLAY_INTERFACE_H__
#define __DISPLAY_INTERFACE_H__

	#include <stdint.h>

	/**
	 * defines the monotone colour type
	 */
	typedef enum monotoneColour {
		ColourOff = 0,
		ColourMid = 0x07,
		ColourOn = 0x0F
	} monotoneColour;

	/**
	 * Creates a common comm interface
	 */
	typedef struct GenericComInterface {
		void (*Open) (void);
		void (*Close) (void);
		void (*Write) (uint8_t * source, uint32_t length);
		void (*Read) (uint8_t * destination, uint32_t length);
	} GenericComInterface;

	/**
	 * defines an interface layer for displays
	 */
	typedef struct DisplayInterfaceType {
		void (*Open) (GenericComInterface * interface);
		void (*Close) (uint_fast8_t cleanScreenFlag);
		void (*Sync)(void);
		void (*SetPixel)(uint32_t x, uint32_t y, uint8_t value);
		void (*Clear)(void);
		void (*Fill)(monotoneColour value);
	} DisplayInterfaceType;


#endif /* __DISPLAY_INTERFACE_H__ */
