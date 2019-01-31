/*
 * ssd1306.c
 *
 *  Created on: Dec 14, 2018
 *      Author: Ronald Sousa https://hashdefineelectronics.com
 */
#include "ssd1306.h"

/*
 * defines the screen data size. This is how many pixels are grouped
 */
#define SCREEN_DATA_SIZE 8
/**
 * Defines the screen height.
 */
#define SCREEN_HEIGHT 32
/**
 * Defines the screen Width.
 */
#define SCREEN_WIDTH 128
/**
 * Defines how many pages this screen hase.
 */
#define PAGES (SCREEN_HEIGHT / SCREEN_DATA_SIZE)
/**
 * defines the screen data buffer size
 */
#define DISPLAY_BUFFER_SIZE ((SCREEN_HEIGHT * SCREEN_WIDTH) / SCREEN_DATA_SIZE)

/**
 * this is the screen display buffer
 */
static uint8_t DisplayBuffer[DISPLAY_BUFFER_SIZE];

/**
 * this screen COM instance
 */
static I2CInterface * Interface;

/**
 * defines the various address mode.
 */
typedef enum {
	MODE_HorizontalMode = 0x00,
	MODE_VerticalMode = 0x01,
	MODE_PageMode = 0x02
} AddressMode;

/**
 * Defines the supported driver commands
 */
typedef enum {
	CMD_ChargePump = 0x8D, // enabled or disabled the charge pump 0b00010X00 X = 0 = disabled, 1 = enabled
	/// Fundamental Command Table
	CMD_SetContrasControl = 0x81,
	CMD_SetEntireDisplay = 0xA4, ///< Set the entire display to all on or to follow the graphics RAM. 0b1010010X X = 0 Follow Graphics display, 1 = Turn all pixels
	CMD_EntireDisplayOnIgnoreRam = 0xA5,
	CMD_SetNormalDisplay = 0xA6,
	CMD_SetDisplayOnOrOff = 0xAE,

	// scrolling Commands
	CMD_DeactiavateScroll= 0x2E,

	// addressing setting command
	CMD_SetLowerStartColumn = 0x00, 				///< Set the address lower Column  0x0X where X is anywhere between 0 - F;
	CMD_SetHigherStartColumn = 0x10, 				///< Set the address lower Column  0x1X where X is anywhere between 0 - F;
	CMD_SetMemoryAddressMode = 0x20, 				///< Set the memory mode 0b001000XX where XX can be 00b = Horizontal,  01b = Vertical, 10b = page
	CMD_SetColumnAddress = 0x21,					///< set the coloum start and end address
	CMD_SetPageAddress = 0x22,						///< set the page start and end address
	CMD_SetPageStartAddressForPageMode = 0xB0,		///< Set the start address for page mode. 0b00100XXX where XXX is the page

	// Hardware Configuration
	CMD_SetDisplayStartLine = 0x40, ///< Set the display start line. 0b01XXXXXX where XXXXXX = 0 to 63. Default 0
	CMD_SetSegmentRemap = 0xA0,	///< Set the segment remap. 0xAX where X is 0 = Column address 0 remapped to SEG0, 1 = Column address 127 remapped to SEG0

	CMD_SetMultiplexRatio = 0XA8,
	CMD_SetComOutputScanDirection = 0XC0, ///< Set the scan direction. 0b0000X000 X = 0 normal mode COM0 - COM[n-1], 1 = Invert COM[n-1] - COM[0]
	CMD_SetDisplayOffset = 0xD3, ///< Set the display offset
	CMD_SetComPinsHardwareConfiguration = 0xDA, ///< Set the com configuration 0b00AB0010 where A 0 = Sequential, 1 = Alternative, B 0 = Disabled, 1 = Enabled

	CMD_SetDisplayClockDived = 0xD5,
	CMD_SetPreChargePeriod = 0xD9,
	CMD_SetVComHDeselect = 0xDB,
	CMD_NOP = 0xE3,

} SSD1306CommandType;

/**
 * this holds the configuration data for the Oled
 */
static uint8_t DisplayInit[] = {
		CMD_SetDisplayOnOrOff, // set display to off

		CMD_SetDisplayClockDived,
		0x80,
		CMD_SetMultiplexRatio,
		0x1F, // SCREEN_HEIGHT -1

		CMD_SetDisplayOffset,
		0x00,

		CMD_SetDisplayStartLine,

		CMD_ChargePump,
		0x14,

		CMD_SetMemoryAddressMode,
		MODE_HorizontalMode,

		CMD_SetColumnAddress,
		0x00,
		0x7F,
		CMD_SetPageAddress,
		0x00,
		PAGES - 1,

		CMD_SetSegmentRemap | 0x01,

		CMD_SetComOutputScanDirection | 0x08,

		CMD_SetComPinsHardwareConfiguration | 0x02,

		CMD_SetContrasControl,
		0xFF,

		CMD_SetPreChargePeriod,
		0xF1,

		CMD_SetVComHDeselect,
		0x40,

		CMD_SetEntireDisplay,

		CMD_SetNormalDisplay,

		CMD_DeactiavateScroll,

		CMD_SetDisplayOnOrOff | 0x01,
		CMD_SetPageStartAddressForPageMode


};

/**
 * this is the commands needed to reset the screen pointer
 */
static uint8_t ResetPointer[] = {
		CMD_SetColumnAddress,
		0x00,
		0x7F,
		CMD_SetPageAddress,
		0x00,
		(PAGES - 1)
};

/**
 * set the continuation bit Co
 */
#define SET_CONTINUATION_BIT 0x80

/**
 * macro for setting the D/C# portion of the to data type
 */
#define SET_DC_TO_DATA 0x40

/**
 * macro for setting the D/C# portion of the to command type
 */
#define SET_DC_TO_COMMAND 0X00

/**
 * handles sending commands to the Oled display
 *
 * @param command this is the command to send
 */
static void SendGroupOfCommand(uint8_t *data, uint8_t length) {
	uint8_t Buffer[100];
	uint8_t *BufferPointer;
	uint8_t LengthToSend = 1; // at least one command and a DC byte

	if(!Interface) {
		// error. no interface found
		return;
	}
	Buffer[0] = (SET_DC_TO_COMMAND);
	BufferPointer = &Buffer[1];

	if(data && length) {

		for( ; length ; length--) {

			*BufferPointer =  *data;
			BufferPointer++;
			data++;
			LengthToSend++;
		}
	}

	Interface->Write(&Buffer[0], LengthToSend);

}

/**
 * handles sending commands to the Oled display
 *
 * @param command this is the command to send
 */
static void SendDisplay(uint8_t *data, uint32_t length) {
	uint8_t Buffer[DISPLAY_BUFFER_SIZE + 10];
	uint8_t *BufferPointer;
	uint32_t LengthToSend = 1; // at least one command and a DC byte

	if(!Interface) {
		// error. no interface found
		return;
	}
	Buffer[0] = (SET_DC_TO_DATA);
	BufferPointer = &Buffer[1];

	if(data && length) {

		for( ; length ; length--) {

			*BufferPointer =  *data;
			BufferPointer++;
			data++;
			LengthToSend++;
		}
	}

	// reset the pointer
	SendGroupOfCommand(&ResetPointer[0], sizeof(ResetPointer));

	Interface->Write(&Buffer[0], LengthToSend);

}


/**
 * this function handle flushing out the Buffer content in to the screen
 */
static void Sync(void) {

	SendDisplay(&DisplayBuffer[0], DISPLAY_BUFFER_SIZE);
}

/**
 * this will fill the buffer content.
 *
 * @param value if true = all pixel on, else all pixel off
 *
 * @note you will need to call ssd1306_Sync() to push the changes to the screen
 */
static void Fill(monotoneColour value) {
	// select to either fill all of clear all
	value = value ? 0xFF : 0x00;

	// clear the buffer
	memset(&DisplayBuffer[0], value, DISPLAY_BUFFER_SIZE);
}

/**
 * this will clear the buffer content.
 *
 * @note you will need to call ssd1306_Sync() to push the changes to the screen
 */
static void Clear(void) {
	Fill(ColourOff);
}

/**
 * call this function when you're done turn off the screen and terminal all COM with it.
 *
 * @param cleanScreenFlag
 */
static void Close(uint_fast8_t cleanScreenFlag) {
	if(!Interface) {
		// error this is an invalid pointer
		return;
	}
	if(cleanScreenFlag) {
		Clear();
		Sync();
	}

	Interface->Close();
}

/**
 * this function handle drawing a pixel to the buffer
 */
static void SetPixel(uint32_t y, uint32_t x, uint8_t value) {
	uint32_t PageOffset;

	if(y > (SCREEN_WIDTH - 1)) {
		y = (SCREEN_WIDTH - 1);
	}

	if(x > (SCREEN_HEIGHT - 1)) {
		x = (SCREEN_WIDTH - 1);
	}

	// this operation is meant to discard the decimal points
	PageOffset = (x / SCREEN_DATA_SIZE);

	// before we work out the final page offset, lets calculate the bit fields offset
	x = (x - (PageOffset * SCREEN_DATA_SIZE)) & 0xFF;

	// calculate the offset
	PageOffset *= SCREEN_WIDTH;




	if(value) {
		DisplayBuffer[y + PageOffset] |= (0x01 << x);
	} else {
		DisplayBuffer[y + PageOffset] &= ~(0x01 << x);
	}

}


/**
 * Configures the display
 */
static void Open(GenericComInterface * interface) {

	if(!interface) {
		// error this is an invalid pointer
		return;
	}

	Interface = (I2CInterface*)interface;

	// Configure the display
	SendGroupOfCommand(&DisplayInit[0], sizeof(DisplayInit));

	Clear();
	Sync();
}



/**
 * defines the SSD1306 driver layer
 */
struct DisplayInterfaceType SSD1306 = {
	Width: SCREEN_WIDTH,
	Height: SCREEN_HEIGHT,
	Open: Open,
	Close: Close,
	Sync: Sync,
	SetPixel: SetPixel,
	Clear: Clear,
	Fill: Fill
};
