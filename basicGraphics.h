/*
 * basicGraphics.h
 *
 *  Created on: Dec 18, 2018
 *      Author: Ronald Susa
 */

#ifndef __BASIC_GRAPHICS_H__
#define __BASIC_GRAPHICS_H__

	#include <string.h>
	#include <stdio.h>
	#include <stdint.h>
	#include "stdbool.h"
	#include "Fonts/gfxfont.h"
	#include "displayDriver.h"

	/**
	 * Defines the standard graphics functuion return types.
	 */
	typedef enum GraphicsReturnType {
		BasicGReturned_OK = 0,
		BasicGReturned_Error,
		RBasicGReturned_InvalidPointer
	} GraphicsReturnType;

	/**
	 * defines our simple graphics structure
	 */
	typedef struct SimpleGraphcisType {
		void (*Init) (DisplayInterfaceType * driver, const GFXfont * font);
		void (*Destroy) (void);
		void (*Clear)(void);
		void (*Flush)(void);
		void (*WriteString)(uint8_t * text, uint_fast8_t xPos, uint_fast8_t yPos, uint_fast8_t colour);
	} SimpleGraphcisType;


	extern SimpleGraphcisType GraphicsInstance;

#endif /* __BASIC_GRAPHICS_H__ */
