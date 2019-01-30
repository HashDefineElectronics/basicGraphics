/*
 * basicGraphics.c
 *
 *  Created on: Dec 18, 2018
 *      Author: Ronald Sousa https://hashdefineelectronics.com
 *
 * This is the a simple graphics library for drawing primitives, fonts and bitmaps
 */
#include "basicGraphics.h"

/**
 * Holds the current fonts
 */
static const GFXfont * CurrentFont =  ((void*)0);

/**
 * Holds the display driver
 */
static DisplayInterfaceType *Driver;

/**
 * configure our graphics display
 */
static void Init(DisplayInterfaceType * driver, const GFXfont * font) {

	if(!driver || !font) {
		return;
	}

	CurrentFont = font;
	Driver = driver;
}

/**
 * this function will return the give text height and width bound.
 *
 * @param text is the pointer to the string to find screen bound
 * @param font is the font pointer
 * @param bounds pointer to return the new bound values
 */
static void getStringBounds(uint8_t * text, const GFXfont * font, basicStringBoundType * bounds) {
	GFXglyph *Glyph;
	uint8_t  *Bitmap;
	uint8_t TempChar;
	GFXfont * Font = font;

	uint32_t YTemp;
	uint32_t XTemp;

	bounds->x = 0;
	bounds->y = 0;
	bounds->xMin = 0;
	bounds->yMin = 0;
	bounds->xMax = 0;
	bounds->yMax = 0;

	if(!bounds) {
		return;
	}

	if(!Font) {
		if(!CurrentFont) {
			/// @TODO return invalid pointer error
			return;
		}
		Font = CurrentFont;
	}

	while(*text) {
		TempChar = *text;

		// make sure that we can render the character
		if(TempChar != '\r' && TempChar >= Font->first && TempChar <= Font->last ) {

			TempChar -= Font->first;
			Glyph  = &Font->glyph[TempChar];

			XTemp = bounds->x + Glyph->xOffset;
			if(XTemp < bounds->xMin) {
				 bounds->xMin = XTemp;
			}

			XTemp += Glyph->width;
			if(XTemp > bounds->xMax) {
				 bounds->xMax = XTemp;
			}
			bounds->x += Glyph->xAdvance;

			YTemp = bounds->y +  Glyph->yOffset;
			if(YTemp < bounds->yMin) {
				 bounds->yMin = YTemp;
			}

			YTemp += Glyph->height;
			if(YTemp > bounds->yMax) {
				 bounds->yMax = YTemp;
			}
		}

		text++;
	}
}
/**
 * handles rendering the character with the given font.
 *
 * @param glyph pointer to the character data structure
 * @param bitmap pointer to the character bitmap data buffer
 * @param character the ascii character to render
 * @param xPos pointer to the X axis position
 * @param yPos pointer to the Y axis position
 * @param colour this is the colour that the text will be drawn as. Current only support monotone which is TRUE or FALSE.
 */
static GraphicsReturnType renderCharacter(	GFXglyph *glyph,
											uint8_t  *bitmap,
											uint8_t character,
											uint32_t *xPos,
											uint32_t *yPos,
											uint_fast8_t colour) {

	if(!CurrentFont || !Driver || !Driver->SetPixel) {
		// invalid pointer
		return RBasicGReturned_InvalidPointer;
	}

	uint_fast8_t Height = glyph->height;
	uint_fast8_t Width = glyph->width;

	uint32_t xOffset = glyph->xOffset;
	uint32_t yOffset = glyph->yOffset;

	uint8_t  BitIndex = 0;

	uint16_t SegmentIndex = glyph->bitmapOffset;
	// this has the section of bits that make up the character.
	uint8_t Segment = 0;

	uint32_t YIndex;
	uint32_t XIndex;


	/// @Todo Add character clipping here

	for(YIndex = 0; YIndex < Height; YIndex++) {

		for(XIndex = 0; XIndex < Width; XIndex++) {

			// check if we need to get the next 8bit of data for this character
			if(!(BitIndex++ & 7)) {
				Segment = bitmap[SegmentIndex++];
			}

			if(Segment & 0x80) {
				Driver->SetPixel(*xPos + xOffset + XIndex, *yPos + yOffset + YIndex, colour);
			}

			Segment <<= 1;
		}
	}

	return BasicGReturned_OK;
}

/*
 * Handles drawing string onto the string
 *
 * @param *text is the pointer to a null terminated string
 * @param xPos is the X axis position
 * @param yPos is the Y axis position. Note that yPos is draw from bottom up which mean that the minimum yPos should be the text height
 * @param colour this is the colour that the text will be drawn as. Current only support monotone which is TRUE or FALSE.
 */
static void WriteString(uint8_t * text, uint32_t xPos, uint32_t yPos, uint_fast8_t colour, const GFXfont * fontToUse) {
	GFXglyph *Glyph;
	uint8_t  *Bitmap;
	uint8_t TempChar;
	GFXfont * Font = fontToUse;

	if(!CurrentFont || !Driver || !Driver->SetPixel) {
		/// @TODO return invalid pointer error
		return;
	}

	if(!Font) {
		Font = CurrentFont;
	}

	while(*text) {
		TempChar = *text;

		// make sure that we can render the character
		if(TempChar >= Font->first && TempChar <= Font->last ) {

			TempChar -= Font->first;
			Glyph  = &Font->glyph[TempChar];

			// make sure that we aren't trying to render an empty character
			if(Glyph->width && Glyph->height) {
				Bitmap = Font->bitmap;
				renderCharacter(Glyph, Bitmap, TempChar, &xPos, &yPos, colour);
			}
			xPos += Glyph->xAdvance;
		}

		text++;
	}

}
/**
 * this function should be call to clean and free up resources
 */
static void Destroy(void) {
	if(!Driver || !Driver->Close) {
		return;
	}
	Driver->Close(true);
}

/**
 * Clears the screen buffer
 */
static void Clear(void) {

	if(!Driver || !Driver->Clear) {
		return;
	}
	Driver->Clear();
}

/**
 * Send the current draw buffer to the screen
 */
static void Flush(void) {
	if(!Driver || !Driver->Sync) {
		return;
	}
	Driver->Sync();
}
/**
 * This is our graphics instance
 */
SimpleGraphcisType GraphicsInstance = {
		Init: Init,
		Destroy: Destroy,
		Clear: Clear,
		Flush: Flush,
		WriteString: WriteString,
		GetStringBounds: getStringBounds
};
