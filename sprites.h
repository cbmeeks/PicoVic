#pragma once

#include <stdint-gcc.h>
#include <stdbool.h>

#include "screen_modes.h"
#include "res/sprites/sprite_defs.h"

#define NUMBER_OF_SPRITES 64
#define NUMBER_OF_SPRITE_PALETTES 4

typedef struct {
    uint16_t x;
    uint8_t y;
    uint32_t *frame;
    uint8_t height;
    uint8_t x_speed;
    uint8_t y_speed;
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
void setSprite(uint8_t number, uint16_t x, uint8_t y, uint8_t x_speed, uint8_t y_speed);
void setSpriteFrame(uint8_t number, uint32_t *frame);
void setSpriteVisible(uint8_t number, bool visible);
void setSpriteHeight(uint8_t number, uint8_t height);