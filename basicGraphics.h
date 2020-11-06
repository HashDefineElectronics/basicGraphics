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
	 * this is the justification of a text
	 */
	typedef enum {
		Text_Left = 0,
		Text_Right,
		Text_Center,
	} GraphicsTextPostEnumType;

	/**
	 * defines the data structure for working out the a string bound based on a given font
	 */
	typedef struct {
		uint32_t x;			///< x start position of the string
		uint32_t y;			///< x start position of the string
		uint32_t width;		///< the complete string width
		uint32_t height;	///< the complete string height
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
		void (*Reset) (uint_fast8_t resetBuffer);
		void (*Clear)(void);
		void (*Flush)(void);
		void (*WriteString)(uint8_t * text, uint32_t xPos, uint32_t yPos, uint_fast8_t colour, const GFXfont * fontToUse);
		void (*GetStringBounds)(uint8_t * text, GFXfont * font, basicStringBoundType * bounds);
		void (*getStringJustificationPos)(basicStringBoundType * TextBounds, GraphicsTextPostEnumType justification, uint32_t containerWidth, uint32_t containerHeight);
		void (*drawLine)(int32_t xStart, int32_t yStart, int32_t xEnd, int32_t yEnd, uint_fast8_t colour);
		void (*drawCircle)(int32_t x0, int32_t y0, int32_t radius, uint_fast8_t colour, uint_fast8_t fill);
		void (*drawRectagle)(int32_t xStart, int32_t yStart, int32_t xEnd, int32_t yEnd, uint_fast8_t colour, uint_fast8_t fill);
		void (*drawIcon) (int32_t x, int32_t y, uint32_t height, uint32_t width, uint_fast8_t colour, uint32_t *source);
		void (*Fill)(uint8_t value);
		void (*Update)(void);
	} SimpleGraphcisType;


	extern SimpleGraphcisType GraphicsInstance;

#endif /* __BASIC_GRAPHICS_H__ */
