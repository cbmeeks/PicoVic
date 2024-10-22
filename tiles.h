#pragma once

#include "res/tiles/tile_defs.h"

#define NUMBER_OF_TILES 256
#define TILE_WIDTH 16
#define TILE_HEIGHT 16
#define NUMBER_OF_TILE_PALETTES 16

typedef struct {
    uint16_t color[4];
} TilePalette;


void updateMap();
void drawMap(uint16_t screenWidth, uint16_t screenHeight, uint16_t raster_y, uint16_t pixels[screenWidth]);

void setTilePalette(uint8_t number, uint16_t color0, uint16_t color1, uint16_t color2, uint16_t color3);
