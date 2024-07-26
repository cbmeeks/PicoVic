
#include "tiles.h"
#include "vga.h"

#define __no_inline_not_in_flash_func(func) func

static TilePalette tile_palettes[NUMBER_OF_TILE_PALETTES] = {
        {0x0000, 0x009a, 0x042f, 0x08ef},
        {0x0000, 0x0ff0, 0x04f1, 0x05a5},
        {0x0000, 0x0444, 0x0fff, 0x0f03},
        {0x0000, 0x0888, 0x0f0f, 0x0f37},
};


// Map array (20x15)
uint8_t map[300] = {
        0x01, 0x01, 0x03, 0x03, 0x01, 0x05, 0x01, 0x01, 0x03, 0x01, 0x03, 0x01, 0x01, 0x01, 0x03, 0x01, 0x03, 0x01, 0x01, 0x01,
        0x01, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x10,
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x01,
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05,
        0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x01, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x05, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x01, 0x00, 0x00, 0x02, 0x02, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x01, 0x01, 0x02, 0x02, 0x03, 0x01, 0x01, 0x01, 0x01, 0x05, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x05, 0x01, 0x01, 0x01,
};

uint8_t map_tile_palettes[300] = {
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
};


// This is the ORIGINAL code
void
__no_inline_not_in_flash_func(drawMap)(uint16_t screenWidth, uint16_t screenHeight, uint16_t raster_y, uint16_t pixels[screenWidth + 64]) {
    if (raster_y >= screenHeight) return;

    int map_row_offset = (raster_y / TILE_HEIGHT) * 20;
    int line_offset = raster_y % TILE_HEIGHT;

    for (int x = 0; x < 20; x++) {
        uint8_t tile_number = map[map_row_offset + x];
        uint8_t tile_palette = map_tile_palettes[map_row_offset + x];
        uint32_t line = tiles[tile_number][line_offset];

        uint8_t c0 = ((line & 0b11000000000000000000000000000000) >> 30);
        uint8_t c1 = ((line & 0b00110000000000000000000000000000) >> 28);
        uint8_t c2 = ((line & 0b00001100000000000000000000000000) >> 26);
        uint8_t c3 = ((line & 0b00000011000000000000000000000000) >> 24);
        uint8_t c4 = ((line & 0b00000000110000000000000000000000) >> 22);
        uint8_t c5 = ((line & 0b00000000001100000000000000000000) >> 20);
        uint8_t c6 = ((line & 0b00000000000011000000000000000000) >> 18);
        uint8_t c7 = ((line & 0b00000000000000110000000000000000) >> 16);
        uint8_t c8 = ((line & 0b00000000000000001100000000000000) >> 14);
        uint8_t c9 = ((line & 0b00000000000000000011000000000000) >> 12);
        uint8_t ca = ((line & 0b00000000000000000000110000000000) >> 10);
        uint8_t cb = ((line & 0b00000000000000000000001100000000) >> 8);
        uint8_t cc = ((line & 0b00000000000000000000000011000000) >> 6);
        uint8_t cd = ((line & 0b00000000000000000000000000110000) >> 4);
        uint8_t ce = ((line & 0b00000000000000000000000000001100) >> 2);
        uint8_t cf = ((line & 0b00000000000000000000000000000011) >> 0);

        pixels[x * TILE_WIDTH + 32] = tile_palettes[tile_palette].color[c0];        // add 32 to offset the drawing for sprite borders
        pixels[x * TILE_WIDTH + 1 + 32] = tile_palettes[tile_palette].color[c1];
        pixels[x * TILE_WIDTH + 2 + 32] = tile_palettes[tile_palette].color[c2];
        pixels[x * TILE_WIDTH + 3 + 32] = tile_palettes[tile_palette].color[c3];
        pixels[x * TILE_WIDTH + 4 + 32] = tile_palettes[tile_palette].color[c4];
        pixels[x * TILE_WIDTH + 5 + 32] = tile_palettes[tile_palette].color[c5];
        pixels[x * TILE_WIDTH + 6 + 32] = tile_palettes[tile_palette].color[c6];
        pixels[x * TILE_WIDTH + 7 + 32] = tile_palettes[tile_palette].color[c7];
        pixels[x * TILE_WIDTH + 8 + 32] = tile_palettes[tile_palette].color[c8];
        pixels[x * TILE_WIDTH + 9 + 32] = tile_palettes[tile_palette].color[c9];
        pixels[x * TILE_WIDTH + 10 + 32] = tile_palettes[tile_palette].color[ca];
        pixels[x * TILE_WIDTH + 11 + 32] = tile_palettes[tile_palette].color[cb];
        pixels[x * TILE_WIDTH + 12 + 32] = tile_palettes[tile_palette].color[cc];
        pixels[x * TILE_WIDTH + 13 + 32] = tile_palettes[tile_palette].color[cd];
        pixels[x * TILE_WIDTH + 14 + 32] = tile_palettes[tile_palette].color[ce];
        pixels[x * TILE_WIDTH + 15 + 32] = tile_palettes[tile_palette].color[cf];
    }
}


// This is my first attempt at optimizing
void
__no_inline_not_in_flash_func(drawMapOptimized1)(uint16_t screenWidth, uint16_t screenHeight, uint16_t raster_y,
                                                 uint16_t pixels[screenWidth]) {
    if (raster_y >= screenHeight) return;

    int map_row_offset = (raster_y / TILE_HEIGHT) * 20;
    int line_offset = raster_y % TILE_HEIGHT;

    for (int x = 0; x < 20; x++) {
        uint8_t tile_number = map[map_row_offset + x];
        uint16_t map_tile_palette_idx = map_tile_palettes[map_row_offset + x];
        uint32_t line = tiles[tile_number][line_offset];

        // Pre-calculate the highest pixel address (rightmost) in pixels.
        // Can just decrement this pointer rather than recalculating for each of the 16 pixels.
        uint16_t *pixel = &pixels[x * TILE_WIDTH + 15];

        // Pre-calculate the palette which is appropriate for this tile.
        TilePalette *palette = &tile_palettes[map_tile_palette_idx];

        // Perform the color extraction from line and the pixel update for each pixel at the same time to
        // reduce register pressure. This saves extra memory reads/writes to recover pixel color values from the
        // stack.
        // I updated them in reverse order to simplify the calculation down to right shift by 2-bits and masking off
        // the lower 2-bits (same operation for each pixel).
        // Note, there is no color 0 checking here since a tile will draw color 0.
        uint8_t cf = line & 0b11;
        *pixel = palette->color[cf];

        line >>= 2;
        pixel--;
        uint8_t ce = line & 0b11;
        *pixel = palette->color[ce];

        line >>= 2;
        pixel--;
        uint8_t cd = line & 0b11;
        *pixel = palette->color[cd];

        line >>= 2;
        pixel--;
        uint8_t cc = line & 0b11;
        *pixel = palette->color[cc];

        line >>= 2;
        pixel--;
        uint8_t cb = line & 0b11;
        *pixel = palette->color[cb];

        line >>= 2;
        pixel--;
        uint8_t ca = line & 0b11;
        *pixel = palette->color[ca];

        line >>= 2;
        pixel--;
        uint8_t c9 = line & 0b11;
        *pixel = palette->color[c9];

        line >>= 2;
        pixel--;
        uint8_t c8 = line & 0b11;
        *pixel = palette->color[c8];

        line >>= 2;
        pixel--;
        uint8_t c7 = line & 0b11;
        *pixel = palette->color[c7];

        line >>= 2;
        pixel--;
        uint8_t c6 = line & 0b11;
        *pixel = palette->color[c6];

        line >>= 2;
        pixel--;
        uint8_t c5 = line & 0b11;
        *pixel = palette->color[c5];

        line >>= 2;
        pixel--;
        uint8_t c4 = line & 0b11;
        *pixel = palette->color[c4];

        line >>= 2;
        pixel--;
        uint8_t c3 = line & 0b11;
        *pixel = palette->color[c3];

        line >>= 2;
        pixel--;
        uint8_t c2 = line & 0b11;
        *pixel = palette->color[c2];

        line >>= 2;
        pixel--;
        uint8_t c1 = line & 0b11;
        *pixel = palette->color[c1];

        line >>= 2;
        pixel--;
        uint8_t c0 = line & 0b11;
        *pixel = palette->color[c0];
    }
}


// This is my second attempt at optimizing
void
__no_inline_not_in_flash_func(drawMapOptimized2)(uint16_t screenWidth, uint16_t screenHeight, uint16_t raster_y,
                                                 uint16_t pixels[screenWidth]) {
    if (raster_y >= screenHeight) return;

    int map_row_offset = (raster_y / TILE_HEIGHT) * 20;
    int line_offset = raster_y % TILE_HEIGHT;

    for (int x = 0; x < 20; x++) {
        uint8_t tile_number = map[map_row_offset + x];
        uint8_t tile_palette = map_tile_palettes[map_row_offset + x];
        uint32_t line = tiles[tile_number][line_offset];

        uint8_t c0 = ((line & 0b11000000000000000000000000000000) >> 30);
        uint8_t c1 = ((line & 0b00110000000000000000000000000000) >> 28);
        uint8_t c2 = ((line & 0b00001100000000000000000000000000) >> 26);
        uint8_t c3 = ((line & 0b00000011000000000000000000000000) >> 24);
        uint8_t c4 = ((line & 0b00000000110000000000000000000000) >> 22);
        uint8_t c5 = ((line & 0b00000000001100000000000000000000) >> 20);
        uint8_t c6 = ((line & 0b00000000000011000000000000000000) >> 18);
        uint8_t c7 = ((line & 0b00000000000000110000000000000000) >> 16);
        uint8_t c8 = ((line & 0b00000000000000001100000000000000) >> 14);
        uint8_t c9 = ((line & 0b00000000000000000011000000000000) >> 12);
        uint8_t ca = ((line & 0b00000000000000000000110000000000) >> 10);
        uint8_t cb = ((line & 0b00000000000000000000001100000000) >> 8);
        uint8_t cc = ((line & 0b00000000000000000000000011000000) >> 6);
        uint8_t cd = ((line & 0b00000000000000000000000000110000) >> 4);
        uint8_t ce = ((line & 0b00000000000000000000000000001100) >> 2);
        uint8_t cf = ((line & 0b00000000000000000000000000000011) >> 0);

        uint16_t *pixel = &pixels[x * TILE_WIDTH + 15];

        *pixel = tile_palettes[tile_palette].color[cf];
        pixel--;

        *pixel = tile_palettes[tile_palette].color[ce];
        pixel--;

        *pixel = tile_palettes[tile_palette].color[cd];
        pixel--;

        *pixel = tile_palettes[tile_palette].color[cc];
        pixel--;

        *pixel = tile_palettes[tile_palette].color[cb];
        pixel--;

        *pixel = tile_palettes[tile_palette].color[ca];
        pixel--;

        *pixel = tile_palettes[tile_palette].color[c9];
        pixel--;

        *pixel = tile_palettes[tile_palette].color[c8];
        pixel--;

        *pixel = tile_palettes[tile_palette].color[c7];
        pixel--;

        *pixel = tile_palettes[tile_palette].color[c6];
        pixel--;

        *pixel = tile_palettes[tile_palette].color[c5];
        pixel--;

        *pixel = tile_palettes[tile_palette].color[c4];
        pixel--;

        *pixel = tile_palettes[tile_palette].color[c3];
        pixel--;

        *pixel = tile_palettes[tile_palette].color[c2];
        pixel--;

        *pixel = tile_palettes[tile_palette].color[c1];
        pixel--;

        *pixel = tile_palettes[tile_palette].color[c0];
    }
}


void setTilePalette(uint8_t number, uint16_t color0, uint16_t color1, uint16_t color2, uint16_t color3) {
    if (number < 0 || number >= NUMBER_OF_TILE_PALETTES) return;
    tile_palettes[number].color[0] = color0;
    tile_palettes[number].color[1] = color1;
    tile_palettes[number].color[2] = color2;
    tile_palettes[number].color[3] = color3;
}


void updateMap() {
}





