#include "sprites.h"


/*
 * Sprites
 */
Sprite sprites[NUMBER_OF_SPRITES] = {};
SpritePalette sprite_palettes[NUMBER_OF_SPRITE_PALETTES] = {
        {0x0000, 0x00ff, 0x0fa4, 0x00f0},
        {0x0000, 0x0ff0, 0x04f1, 0x05a5},
        {0x0000, 0x0444, 0x0fff, 0x0f03},
        {0x0000, 0x0888, 0x0f0f, 0x0f37},
};

static ScreenModeParams screenModeParams;

void initSprites(ScreenModeParams modeParams) {
    screenModeParams = modeParams;
    for (int s = 0; s < NUMBER_OF_SPRITES; s++) {
        sprites[s].x = 0;
        sprites[s].y = 0;
        sprites[s].frame = blank;
        sprites[s].x_speed = 0;
        sprites[s].y_speed = 0;
        sprites[s].height = 16;
        sprites[s].palette = 0;
        sprites[s].visible = false;
    }
}

void drawSprites(uint16_t screenWidth, uint16_t screenHeight, uint16_t raster_y, uint16_t pixels[screenWidth]) {
    for (int s = 0; s < NUMBER_OF_SPRITES; s++) {
        if (sprites[s].visible &&
            raster_y >= sprites[s].y &&
            raster_y < (sprites[s].y + sprites[s].height) &&
            raster_y < screenModeParams.vga_virtual_pixel_height) {

            int offset = (raster_y - sprites[s].y);
            uint32_t line = sprites[s].frame[offset];

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

            if (c0 != 0 && sprites[s].x + 0 >= 0 && sprites[s].x + 0 < screenWidth)
                pixels[sprites[s].x + 0] = sprite_palettes[sprites[s].palette].color[c0];

            if (c1 != 0 && sprites[s].x + 1 >= 0 && sprites[s].x + 1 < screenWidth)
                pixels[sprites[s].x + 1] = sprite_palettes[sprites[s].palette].color[c1];

            if (c2 != 0 && sprites[s].x + 2 >= 0 && sprites[s].x + 2 < screenWidth)
                pixels[sprites[s].x + 2] = sprite_palettes[sprites[s].palette].color[c2];

            if (c3 != 0 && sprites[s].x + 3 >= 0 && sprites[s].x + 3 < screenWidth)
                pixels[sprites[s].x + 3] = sprite_palettes[sprites[s].palette].color[c3];

            if (c4 != 0 && sprites[s].x + 4 >= 0 && sprites[s].x + 4 < screenWidth)
                pixels[sprites[s].x + 4] = sprite_palettes[sprites[s].palette].color[c4];

            if (c5 != 0 && sprites[s].x + 5 >= 0 && sprites[s].x + 5 < screenWidth)
                pixels[sprites[s].x + 5] = sprite_palettes[sprites[s].palette].color[c5];

            if (c6 != 0 && sprites[s].x + 6 >= 0 && sprites[s].x + 6 < screenWidth)
                pixels[sprites[s].x + 6] = sprite_palettes[sprites[s].palette].color[c6];

            if (c7 != 0 && sprites[s].x + 7 >= 0 && sprites[s].x + 7 < screenWidth)
                pixels[sprites[s].x + 7] = sprite_palettes[sprites[s].palette].color[c7];

            if (c8 != 0 && sprites[s].x + 8 >= 0 && sprites[s].x + 8 < screenWidth)
                pixels[sprites[s].x + 8] = sprite_palettes[sprites[s].palette].color[c8];

            if (c9 != 0 && sprites[s].x + 9 >= 0 && sprites[s].x + 9 < screenWidth)
                pixels[sprites[s].x + 9] = sprite_palettes[sprites[s].palette].color[c9];

            if (ca != 0 && sprites[s].x + 10 >= 0 && sprites[s].x + 10 < screenWidth)
                pixels[sprites[s].x + 10] = sprite_palettes[sprites[s].palette].color[ca];

            if (cb != 0 && sprites[s].x + 11 >= 0 && sprites[s].x + 11 < screenWidth)
                pixels[sprites[s].x + 11] = sprite_palettes[sprites[s].palette].color[cb];

            if (cc != 0 && sprites[s].x + 12 >= 0 && sprites[s].x + 12 < screenWidth)
                pixels[sprites[s].x + 12] = sprite_palettes[sprites[s].palette].color[cc];

            if (cd != 0 && sprites[s].x + 13 >= 0 && sprites[s].x + 13 < screenWidth)
                pixels[sprites[s].x + 13] = sprite_palettes[sprites[s].palette].color[cd];

            if (ce != 0 && sprites[s].x + 14 >= 0 && sprites[s].x + 14 < screenWidth)
                pixels[sprites[s].x + 14] = sprite_palettes[sprites[s].palette].color[ce];

            if (cf != 0 && sprites[s].x + 15 >= 0 && sprites[s].x + 15 < screenWidth)
                pixels[sprites[s].x + 15] = sprite_palettes[sprites[s].palette].color[cf];
        }
    }
}


void updateSprites() {
    for (int s = 0; s < NUMBER_OF_SPRITES; s++) {
        sprites[s].x = sprites[s].x + sprites[s].x_speed;
        sprites[s].y = sprites[s].y + sprites[s].y_speed;

        if (sprites[s].x > screenModeParams.vga_virtual_pixel_width) sprites[s].x = 0;
    }
}

void setSprite(uint8_t number, uint16_t x, uint8_t y, uint8_t x_speed, uint8_t y_speed) {
    if (number >= NUMBER_OF_SPRITES) return;
    sprites[number].x = x;
    sprites[number].y = y;
    sprites[number].x_speed = x_speed;
    sprites[number].y_speed = y_speed;
}

void setSpriteFrame(uint8_t number, uint32_t *frame) {
    if (number >= NUMBER_OF_SPRITES) return;
    sprites[number].frame = frame;
}

void setSpriteHeight(uint8_t number, uint8_t height) {
    if (number >= NUMBER_OF_SPRITES) return;
    sprites[number].height = height;
}

void setSpriteVisible(uint8_t number, bool visible) {
    if (number >= NUMBER_OF_SPRITES) return;
    sprites[number].visible = visible;
}
