#pragma once

#include "res/tiles/tile_defs.h"

#define NUMBER_OF_TILES 256
#define TILE_WIDTH 16
#define TILE_HEIGHT 16
#define NUMBER_OF_TILE_PALETTES 16

typedef struct {
    uint16_t color[4];
} TilePalette;


// Map is in Y x X format (height x width)
extern uint8_t map[15][20];

// Each tile position within the map can have it's own palette number.
extern uint8_t map_tile_palettes[15][20];

void drawMap(uint16_t screenWidth, uint16_t screenHeight, uint16_t raster_y, uint16_t pixels[screenWidth]);