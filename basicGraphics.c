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
static void WriteString(uint8_t * text, uint32_t xPos, uint32_t yPos, uint_fast8_t colour) {
	GFXglyph *Glyph;
	uint8_t  *Bitmap;
	uint8_t TempChar;

	if(!CurrentFont || !Driver || !Driver->SetPixel) {
		/// @TODO return invalid pointer error
		return;
	}

	while(*text) {
		TempChar = *text;

		// make sure that we can render the character
		if(TempChar >= CurrentFont->first && TempChar <= CurrentFont->last ) {

			TempChar -= CurrentFont->first;
			Glyph  = &CurrentFont->glyph[TempChar];

			// make sure that we aren't trying to render an empty character
			if(Glyph->width && Glyph->height) {
				Bitmap = CurrentFont->bitmap;
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
		Init,
		Destroy,
		Clear,
		Flush,
		WriteString,
};
