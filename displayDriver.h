/*
 * display.h
 *
 *  Created on: Dec 18, 2018
 *      Author: Ronald Sousa
 */

#ifndef __DISPLAY_INTERFACE_H__
#define __DISPLAY_INTERFACE_H__

	#include "../../common.h"
	#include "../i2cInterface.h"
	/**
	 * defines an interface layer for displays
	 */
	typedef struct DisplayInterfaceType {
		void (*Open) (I2CInterface * interface, uint8_t slaveAddress);
		void (*Close) (uint_fast8_t cleanScreenFlag);
		void (*Sync)(void);
		void (*SetPixel)(uint32_t x, uint32_t y, uint8_t value);
		void (*Clear)(void);
		void (*Fill)(uint_fast8_t value);
	} DisplayInterfaceType;


#endif /* __DISPLAY_INTERFACE_H__ */
