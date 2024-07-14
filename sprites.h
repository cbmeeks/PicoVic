#pragma once

#include <stdint-gcc.h>
#include <stdbool.h>

#include "screen_modes.h"
#include "res/sprites/sprite_defs.h"

#define NUMBER_OF_SPRITES 64
#define NUMBER_OF_SPRITE_PALETTES 16

typedef struct {
    int x;
    int y;
    uint32_t *frame;
    uint8_t height;
    uint8_t width;
    int x_speed;
    int y_speed;
    uint8_t palette;
    bool visible;
} Sprite;

typedef struct {
    uint16_t color[4];
} SpritePalette;

extern Sprite sprites[];
extern SpritePalette sprite_palettes[];

void initSprites(ScreenModeParams modeParams);
void drawSprites(uint16_t screenWidth, uint16_t screenHeight, uint16_t raster_y, uint16_t pixels[screenWidth]);

void updateSprites();
void setSprite(uint8_t number, int x, int y, int x_speed, int y_speed);
void setSpriteFrame(uint8_t number, uint32_t *frame);
void setSpritePalette(uint8_t number, uint8_t palette_number);
void setSpritePosition(uint8_t number, int x, int y);
void setSpriteVisible(uint8_t number, bool visible);
void setSpriteSize(uint8_t number, uint8_t width, uint8_t height);