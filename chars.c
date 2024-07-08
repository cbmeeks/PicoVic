#include <stdint-gcc.h>
#include "chars.h"
#include "res/fonts/petscii/petscii.h"

uint8_t cursor_x, cursor_y;
uint8_t text_buffer[TEXT_MODE_COUNT];
uint8_t text_fg_clut[TEXT_MODE_COUNT];
uint8_t text_bg_clut[TEXT_MODE_COUNT];
uint16_t palette_buffer[PALETTECOUNT];

void drawChars(uint16_t raster_y, uint16_t screenHeight, uint16_t pixels[]) {
    if (raster_y >= 0 && raster_y < screenHeight) {
        // Grab row index based off current raster Y position
        uint8_t rowIdx = raster_y / CHAR_HEIGHT;
        uint8_t lineIdx = raster_y % 8;

        for (uint16_t colIdx = 0; colIdx < (TEXT_MODE_WIDTH * CHAR_WIDTH); colIdx += CHAR_WIDTH) {
            uint8_t charIdx = colIdx / CHAR_WIDTH;
            uint16_t bufferIdx = rowIdx * TEXT_MODE_WIDTH + charIdx;
            uint8_t character = text_buffer[bufferIdx];
            unsigned char line = petscii[character][lineIdx];
            uint16_t fgcolor = palette_buffer[text_fg_clut[bufferIdx]];
            uint16_t bgcolor = palette_buffer[text_bg_clut[bufferIdx]];

            pixels[colIdx + 0] = CHECK_BIT(line, (7 - 0)) ? fgcolor : bgcolor;
            pixels[colIdx + 1] = CHECK_BIT(line, (7 - 1)) ? fgcolor : bgcolor;
            pixels[colIdx + 2] = CHECK_BIT(line, (7 - 2)) ? fgcolor : bgcolor;
            pixels[colIdx + 3] = CHECK_BIT(line, (7 - 3)) ? fgcolor : bgcolor;
            pixels[colIdx + 4] = CHECK_BIT(line, (7 - 4)) ? fgcolor : bgcolor;
            pixels[colIdx + 5] = CHECK_BIT(line, (7 - 5)) ? fgcolor : bgcolor;
            pixels[colIdx + 6] = CHECK_BIT(line, (7 - 6)) ? fgcolor : bgcolor;
            pixels[colIdx + 7] = CHECK_BIT(line, (7 - 7)) ? fgcolor : bgcolor;
        }
    }
}

void initCharMode() {
    initPalette();
    for (uint16_t i = 0; i < TEXT_MODE_COUNT; i++) {
        text_buffer[i] = 32;    // clear to space
        text_fg_clut[i] = 1;
    }
}


/**
 * Initialized the palette buffer.
 * There are #PALETTECOUNT entries.
 * Each containing a 12-bit color defined as: 0x0RGB for a total of 4096 colors.
 * Just like and OCS Amiga but with more palette entries.  :-)
 */
void initPalette() {
    for (int i = 0; i < PALETTECOUNT; i++) {
        palette_buffer[i] = 0x025b;
    }
    // define some basic values
    palette_buffer[1] = 0x06bf;
}


/**
 * Updates the screen cursor position based on character cells
 * @param x screen x column
 * @param y screen y row
 */
void setTextCursor(uint8_t x, uint8_t y) {
    cursor_x = x;
    cursor_y = y;
}


/**
 * Defines the color definition of a palette entry.
 * @param paletteNumber Palette entry to update
 * @param color Color definition in 0x0RGB format
 */
void setPalette(uint8_t paletteNumber, uint16_t color) {
    if (paletteNumber >= 0 && paletteNumber < PALETTECOUNT - 1) {
        palette_buffer[paletteNumber] = color;
    }
}


/**
 * Changes the text buffer foreground color at position X/Y to one of the palette entries.
 * @param x X position in characters (COLS)
 * @param y Y position in characters (ROWS)
 * @param paletteNumber Palette entry number
 */
void setFGColor(uint8_t x, uint8_t y, uint8_t paletteNumber) {
    text_fg_clut[y * TEXT_MODE_WIDTH + x] = paletteNumber;
}


/**
 * Changes the text buffer background color at position X/Y to one of the palette entries.
 * @param x X position in characters (COLS)
 * @param y Y position in characters (ROWS)
 * @param paletteNumber Palette entry number
 */

void setBGColor(uint8_t x, uint8_t y, uint8_t paletteNumber) {
    text_bg_clut[y * TEXT_MODE_WIDTH + x] = paletteNumber;
}


/**
 * Draws a character to the text buffer
 * @param x X position in characters (column)
 * @param y Y position in characters (rows)
 * @param character Character (from PETSCII font)
 */
void drawCharacter(uint8_t x, uint8_t y, uint8_t character) {
    uint16_t idx = y * TEXT_MODE_WIDTH + x;
    if (idx >= TEXT_MODE_COUNT) return;
    text_buffer[idx] = character;
}

/**
 * Draws an ASCII character to the text buffer (uses ascii_to_petscii buffer)
 * @param x X position in characters (column)
 * @param y Y position in characters (rows)
 * @param character Character (from ASCII to PETSCII buffer)
 */
void drawAsciiCharacter(uint8_t x, uint8_t y, uint8_t character) {
    uint16_t idx = y * TEXT_MODE_WIDTH + x;
    if (idx >= TEXT_MODE_COUNT) return;
    text_buffer[idx] = ascii_to_petscii[character];
}


/**
 * Draws a string of ASCII characters that are mapped to PETSCII characters.
 * NOTE, only the basic upper-case alpha characters, numbers and a few symbols are supported.
 * Also, drawing starts at the current cursor position and will scroll the screen up if it wraps
 * passed the last character position (lower right)
 *
 * @param str String to print out
 */
void drawCharacterString(char *str) {
    while (*str) {
        text_write(*str++);
    }
}

void text_write(unsigned char c) {
    if (c == '\n') {
        // newline pushes cursor down but not to the left
        cursor_y++;
        if (cursor_y >= TEXT_MODE_HEIGHT) {
            cursor_y = TEXT_MODE_HEIGHT - 1;
            shiftCharactersUp();
        }
    } else if (c == '\r') {
        // carriage return pushes cursor down and to the left 0 position
        cursor_x = 0;
        cursor_y++;
        if (cursor_y >= TEXT_MODE_HEIGHT) {
            cursor_y = TEXT_MODE_HEIGHT - 1;
            shiftCharactersUp();
        }
    } else if (c == '\t') {
        // tab moves the cursor over TAB_SPACES.  It will wrap around and adjust the screen.
        cursor_x = cursor_x + TAB_SPACES;
        if (cursor_x >= TEXT_MODE_WIDTH) {
            cursor_x = cursor_x % TEXT_MODE_WIDTH;
            cursor_y++;

            if (cursor_y >= TEXT_MODE_HEIGHT) {
                shiftCharactersUp();
                cursor_y = TEXT_MODE_HEIGHT - 1;
            }
        }
    } else {
        drawAsciiCharacter(cursor_x, cursor_y, c);
        cursor_x++;
        if (cursor_x >= TEXT_MODE_WIDTH) {
            cursor_x = 0;
            cursor_y++;

            if (cursor_y >= TEXT_MODE_HEIGHT) {
                shiftCharactersUp();
                cursor_y = TEXT_MODE_HEIGHT - 1;
            }
        }
        // TODO handle wrapping screen
        // hint: reset cursor_x to 0 and use screen shift up if cursor_x > max x/y cursor position (bottom right)
    }
}


/**
 * Shifts the entire text buffer screen up one line.
 * Destroys the very top line and draws a blank line at the bottom.
 *
 * TODO look into using memcopy for this!!!
 */
void shiftCharactersUp() {
    for (int y = 1; y < TEXT_MODE_HEIGHT; y++) {
        for (int x = 0; x < TEXT_MODE_WIDTH; x++) {
            text_buffer[((y - 1) * TEXT_MODE_WIDTH) + x] = text_buffer[(y * TEXT_MODE_WIDTH) + x];
            text_fg_clut[((y - 1) * TEXT_MODE_WIDTH) + x] = text_fg_clut[(y * TEXT_MODE_WIDTH) + x];
            text_bg_clut[((y - 1) * TEXT_MODE_WIDTH) + x] = text_bg_clut[(y * TEXT_MODE_WIDTH) + x];
        }
    }

    // now draw a blank line at the bottom with the current foreground/background color
    for (int i = 0; i < TEXT_MODE_WIDTH; i++) {
        setFGColor(i, TEXT_MODE_HEIGHT - 1, 1);
        setBGColor(i, TEXT_MODE_HEIGHT - 1, 0);
        drawCharacter(i, TEXT_MODE_HEIGHT - 1, BLANK_CHAR);
    }
}
