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