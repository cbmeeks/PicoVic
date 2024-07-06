

#include "tiles.h"

static TilePalette tile_palettes[NUMBER_OF_TILE_PALETTES] = {
        {0x0000, 0x009a, 0x042f, 0x08ef},
        {0x0000, 0x0ff0, 0x04f1, 0x05a5},
        {0x0000, 0x0444, 0x0fff, 0x0f03},
        {0x0000, 0x0888, 0x0f0f, 0x0f37},
};

// Map array
uint8_t map[15][20] = {
        {0x01, 0x01, 0x03, 0x03, 0x01, 0x05, 0x01, 0x01, 0x03, 0x01, 0x03, 0x01, 0x01, 0x01, 0x03, 0x01, 0x03, 0x01, 0x01, 0x01,},
        {0x01, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x10,},
        {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x01,},
        {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,},
        {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05,},
        {0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,},
        {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,},
        {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,},
        {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,},
        {0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,},
        {0x01, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,},
        {0x05, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,},
        {0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,},
        {0x01, 0x00, 0x00, 0x02, 0x02, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,},
        {0x01, 0x01, 0x02, 0x02, 0x03, 0x01, 0x01, 0x01, 0x01, 0x05, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x05, 0x01, 0x01, 0x01,},
};

uint8_t map_tile_palettes[15][20] = {
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};


void drawMap(uint16_t screenWidth, uint16_t screenHeight, uint16_t raster_y, uint16_t pixels[screenWidth]) {

    if (raster_y >= 0 && raster_y < screenHeight) {
        uint8_t row = (raster_y / TILE_HEIGHT);

        for (int raster_x = 0; raster_x < screenWidth; raster_x += TILE_WIDTH) {
            uint8_t col = raster_x / TILE_WIDTH;
            uint8_t tileNumber = map[row][col];
            uint8_t tilePalette = map_tile_palettes[row][col];

            uint32_t line = tiles[tileNumber][raster_y % TILE_HEIGHT];

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

            pixels[raster_x + 0] = tile_palettes[tilePalette].color[c0];
            pixels[raster_x + 1] = tile_palettes[tilePalette].color[c1];
            pixels[raster_x + 2] = tile_palettes[tilePalette].color[c2];
            pixels[raster_x + 3] = tile_palettes[tilePalette].color[c3];
            pixels[raster_x + 4] = tile_palettes[tilePalette].color[c4];
            pixels[raster_x + 5] = tile_palettes[tilePalette].color[c5];
            pixels[raster_x + 6] = tile_palettes[tilePalette].color[c6];
            pixels[raster_x + 7] = tile_palettes[tilePalette].color[c7];
            pixels[raster_x + 8] = tile_palettes[tilePalette].color[c8];
            pixels[raster_x + 9] = tile_palettes[tilePalette].color[c9];
            pixels[raster_x + 10] = tile_palettes[tilePalette].color[ca];
            pixels[raster_x + 11] = tile_palettes[tilePalette].color[cb];
            pixels[raster_x + 12] = tile_palettes[tilePalette].color[cc];
            pixels[raster_x + 13] = tile_palettes[tilePalette].color[cd];
            pixels[raster_x + 14] = tile_palettes[tilePalette].color[ce];
            pixels[raster_x + 15] = tile_palettes[tilePalette].color[cf];
        }
    }

}




