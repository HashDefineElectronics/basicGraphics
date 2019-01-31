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
	 * defines the data structure for working out the a string bound based on a given font
	 */
	typedef struct {
		uint32_t x;
		uint32_t y;
		uint32_t xMin;
		uint32_t yMin;
		uint32_t xMax;
		uint32_t yMax;
	} basicStringBoundType;

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
		void (*WriteString)(uint8_t * text, uint32_t xPos, uint32_t yPos, uint_fast8_t colour, const GFXfont * fontToUse);
		void (*GetStringBounds)(uint8_t * text, const GFXfont * font, basicStringBoundType * bounds);
		void (*drawLine)(uint32_t xStart, uint32_t yStart, uint32_t xEnd, uint32_t yEnd, uint_fast8_t colour);
	} SimpleGraphcisType;


	extern SimpleGraphcisType GraphicsInstance;

#endif /* __BASIC_GRAPHICS_H__ */
