/*
 * basicGraphics.c
 *
 *  Created on: Dec 18, 2018
 *      Author: Ronald Sousa https://hashdefineelectronics.com
 *
 * This is the a simple graphics library for drawing primitives, fonts and bitmaps
 */
#include "basicGraphics.h"
#include <stdlib.h>
#include "../common.h"

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


static void getStringJustificationPos(basicStringBoundType * TextBounds, GraphicsTextPostEnumType justification, uint32_t containerWidth, uint32_t containerHeight) {

	switch(justification) {
		case Text_Center:

			if(containerWidth) {
				TextBounds->y = ((containerWidth - TextBounds->width) / 2);
			}

			if(containerHeight) {
				TextBounds->x = ((containerHeight + TextBounds->height) / 2);
			}
			
		break;
		case Text_Left:
			
			if(containerWidth) {
				TextBounds->y = 0;
			}

			if(containerHeight) {
				TextBounds->x = 0;
			}

		break;
		case Text_Right:
			
			if(containerWidth) {
				TextBounds->y = (containerWidth - TextBounds->width);
			}

			if(containerHeight) {
				TextBounds->x = (containerHeight - TextBounds->height);
			}

		break;
	}

}
/**
 * this function will return the give text height and width bound.
 *
 * @param text is the pointer to the string to find screen bound
 * @param font is the font pointer
 * @param bounds pointer to return the new bound values
 */
static void getStringBounds(uint8_t * text, GFXfont * font, basicStringBoundType * bounds) {
	GFXglyph *Glyph;
	uint8_t  *Bitmap;
	uint8_t TempChar;
	GFXfont * Font = font;

	uint32_t YTemp = 0;
	uint32_t XTemp = 0;

	bounds->x = 0;
	bounds->y = 0;
	bounds->width = 0;
	bounds->height = 0;

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


	bounds->height = Font->maxHeight;

	while(*text) {
		TempChar = *text;

		// make sure that we can render the character
		if(TempChar != '\r' && TempChar >= Font->first && TempChar <= Font->last ) {

			TempChar -= Font->first;
			Glyph  = &Font->glyph[TempChar];

			bounds->width += Glyph->xAdvance;
		}

		text++;
	}

	if(Driver && bounds->width > Driver->Width) {
		bounds->width = Driver->Width;
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

	int32_t xOffset = glyph->xOffset;
	int32_t yOffset = glyph->yOffset;

	uint8_t  BitIndex = 0;

	uint16_t SegmentIndex = glyph->bitmapOffset;
	// this has the section of bits that make up the character.
	uint8_t Segment = 0;

	int32_t YIndex;
	int32_t XIndex;


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
 * Sanitases the X and Y coordinates to ensure that it doesn't surpases the screen range
 */
static void SanitisePositionRange(int32_t *x, int32_t *y) {

	if(!Driver || !Driver->SetPixel) {
		return;
	}

	if(*x < 0) {
		*x = 0;
	}

	if(*y < 0) {
		*y = 0;
	}
	if(*x > (Driver->Width - 1)) {
		*x = Driver->Width -1;
	}

	if(*y > (Driver->Height - 1)) {
		*y = Driver->Height -1;
	}
}
/**
 * This function implements the line drawing using Bresenham's line algorithm.
 *
 * @note This code was inspired by https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C
 *
 * @param xStart This is the X start position.
 * @param yStart This is the Y start position.
 * @param xEnd This is the X end position.
 * @param yEnd This is the Y end position.
 * @param colour the pixel colour value
 */
static void drawLine(int32_t xStart, int32_t yStart, int32_t xEnd, int32_t yEnd, uint_fast8_t colour) {

	// make sure that have a driver that we can use
	if(!Driver || !Driver->SetPixel) {
		return;
	}

	SanitisePositionRange(&xStart, &yStart);
	SanitisePositionRange(&xEnd, &yEnd);

	// take the difference between the start and end axis
    int32_t DeltaX = abs(xEnd - xStart);
    int32_t DeltaY = abs(yEnd - yStart);

    // This get which direction we draw the line.
    int32_t OffestX = xStart < xEnd ? 1 : -1;
    int32_t OffestY = yStart < yEnd ? 1 : -1;

    // this is the expected worse error
    int32_t DeltaError = ((DeltaX > DeltaY ? DeltaX : -DeltaY) / 2);

    // this is the current line error
    int32_t Error;

    for(;;) {
        Driver->SetPixel(xStart, yStart, colour);

        if (xStart == xEnd && yStart == yEnd) {
        	break; // we made it to the end point, let get out
        }

        // update our error
        Error = DeltaError;

        // update Y axis error and position
        if (Error > -DeltaX) {
        	DeltaError -= DeltaY;
        	xStart += OffestX;
        }
        // update Y axis error and position
        if (Error < DeltaY) {
        	DeltaError += DeltaX;
        	yStart += OffestY;
        }
    }
}

void drawRectagle(int32_t xStart, int32_t yStart, int32_t xEnd, int32_t yEnd, uint_fast8_t colour, uint_fast8_t fill) {
	// make sure that have a driver that we can use
	int32_t Counter;
	int32_t Diff;
	if(!Driver || !Driver->SetPixel) {
		return;
	}

	SanitisePositionRange(&xStart, &yStart);
	SanitisePositionRange(&xEnd, &yEnd);

	if(fill) {
		Diff = xEnd < xStart ? -1 : 1;

		for( ;xEnd !=  xStart;) {
			drawLine(xStart, yStart, xStart, yEnd, colour);
			xStart += Diff;
		}

	} else {
		drawLine(xStart, yStart, xStart, yEnd, colour);
		drawLine(xStart, yStart, xEnd, yStart, colour);
		drawLine(xEnd, yEnd, xEnd, yStart, colour);
		drawLine(xEnd, yEnd, xStart, yEnd, colour);
	}

}
/**
 * @note this implementation was taken from https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
 *
 * @param x0 circle's x position for its center.
 * @param y0 circle's y position for its center.
 * @param radius circle's radius
 * @param colour the circle colour
 * @param fill if true then fill else just draw the line
 */
void drawCircle(int32_t x0, int32_t y0, int32_t  radius, uint_fast8_t colour, uint_fast8_t fill) {
	// make sure that have a driver that we can use
	if(!Driver || !Driver->SetPixel) {
		return;
	}

	SanitisePositionRange(&x0, &y0);

	int32_t x = radius-1;
	int32_t y = 0;
	int32_t dx = 1;
	int32_t dy = 1;
	int32_t err = dx - (radius << 1);

    while (x >= y) {


    	if(fill) {
			drawLine(x0 + x, y0 + y, x0 - x, y0 + y, colour);
			drawLine(x0 + y, y0 + x, x0 - y, y0 + x, colour);
			drawLine(x0 - x, y0 - y, x0 + x, y0 - y, colour);
			drawLine(x0 - y, y0 - x, x0 + y, y0 - x, colour);
    	} else {
        	Driver->SetPixel(x0 + x, y0 + y, colour);
        	Driver->SetPixel(x0 - x, y0 + y, colour);

        	Driver->SetPixel(x0 + y, y0 + x, colour);
        	Driver->SetPixel(x0 - y, y0 + x, colour);


        	Driver->SetPixel(x0 - x, y0 - y, colour);
        	Driver->SetPixel(x0 + x, y0 - y, colour);

        	Driver->SetPixel(x0 - y, y0 - x, colour);
        	Driver->SetPixel(x0 + y, y0 - x, colour);

    	}

        if (err <= 0) {
            y++;
            err += dy;
            dy += 2;
        }

        if (err > 0) {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}

/**
* this is the data size for the icon array
*/
static const uint_fast8_t IconDataBitSize = 32;

/**
* Draw an icon
* 
* @param x start position x
* @param y start position x
* @param height icon height
* @param width icon width
* @param source	pointer to the data array
*/
static void drawIcon(int32_t x, int32_t y, uint32_t height, uint32_t width, uint_fast8_t colour, uint32_t *source) {
	uint32_t HeightIndex;
	uint32_t WidthIndex;
	uint32_t BitIndex = 0;
	uint32_t Value = *source;

	for(WidthIndex = 0 ; WidthIndex < width; WidthIndex++) {
		for(HeightIndex = 0 ;HeightIndex < height; HeightIndex++) {
			
			Driver->SetPixel(x + HeightIndex, y + WidthIndex, (Value & 0x80000000) ? colour : 0);

			if(BitIndex < IconDataBitSize) {
				BitIndex++;
				Value = Value << 1;
			} else {
				BitIndex = 0;
				source++;
				Value = *source;
			}
			
		}
	}
}

static void Fill(uint8_t value) {
	if(!Driver || !Driver->Fill) {
		return;
	}
	Driver->Fill(value);
}

static void Reset(uint_fast8_t resetBuffer) {
	Driver->Reset(resetBuffer);
}

/**
 * This is our graphics instance
 */
SimpleGraphcisType GraphicsInstance = {
		Init: Init,
		Reset: Reset,
		Destroy: Destroy,
		Clear: Clear,
		Flush: Flush,
		WriteString: WriteString,
		GetStringBounds: getStringBounds,
		getStringJustificationPos : getStringJustificationPos,
		drawLine: drawLine,
		drawCircle: drawCircle,
		drawRectagle: drawRectagle,
		drawIcon : drawIcon,
		Fill: Fill
};
