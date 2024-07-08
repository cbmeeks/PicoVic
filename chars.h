#ifndef __PICOVIC_CHARS_H
#define __PICOVIC_CHARS_H

#include "font_utils.h"
#include "bit_helper.h"


#define TEXT_MODE_WIDTH 40
#define TEXT_MODE_HEIGHT 30
#define TAB_SPACES 4

/**
 * TEXT BUFFER
 * Character buffer used to hold text and other characters.
 * Similar to the $0400 location on a Commodore 64.
 */
#define TEXT_MODE_COUNT (TEXT_MODE_WIDTH * TEXT_MODE_HEIGHT)
#define BLANK_CHAR 32
#define CHAR_WIDTH 8
#define CHAR_HEIGHT 8

extern uint8_t cursor_x, cursor_y;
extern uint8_t text_buffer[];
extern uint8_t text_fg_clut[];
extern uint8_t text_bg_clut[];

/**
 * PALETTE BUFFER
 */
#define PALETTECOUNT 256
extern uint16_t palette_buffer[];


// Text Setup
void initCharMode();
void initCursor();
void initPalette();
void setBGColor(uint8_t x, uint8_t y, uint8_t paletteNumber);
void setFGColor(uint8_t x, uint8_t y, uint8_t paletteNumber);
void setPalette(uint8_t paletteNumber, uint16_t color);
void setTextCursor(uint8_t x, uint8_t y);

// Text Drawing
void drawChars(uint16_t raster_y, uint16_t screenHeight, uint16_t pixels[]);
void drawCharacter(uint8_t x, uint8_t y, uint8_t character);
void drawAsciiCharacter(uint8_t x, uint8_t y, uint8_t character);
void drawCharacterString(char *str);
void text_write(unsigned char c);
void shiftCharactersUp();


#endif