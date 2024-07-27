#include <pico/stdlib.h>
#include "sprites.h"
#include "vga.h"


/*
 * Sprites
 */
static Sprite sprites[NUMBER_OF_SPRITES] = {};
static SpritePalette sprite_palettes[NUMBER_OF_SPRITE_PALETTES] = {
        {0x0000, 0x00ff, 0x0fa4, 0x00f0},
        {0x0000, 0x0e51, 0x0fcb, 0x0e06},
        {0x0000, 0x0444, 0x0fff, 0x0f03},
        {0x0000, 0x0e06, 0x0fcb, 0x0444},
        {0x0000, 0x09fe, 0x0f93, 0x04d4},       // zoomer
        {0x0000, 0x0c41, 0x0fba, 0x0d05},       // Samus no suit
        {0x0000, 0x0fcf, 0x0c0c, 0x0f8f},
};


static VgaParams vgaParams;

void initSprites(VgaParams modeParams) {
    vgaParams = modeParams;
    for (int s = 0; s < NUMBER_OF_SPRITES; s++) {
        sprites[s].x = 0;
        sprites[s].y = 0;
        sprites[s].frame = blank16x16;
        sprites[s].x_speed = 0;
        sprites[s].y_speed = 0;
        sprites[s].height = 16;
        sprites[s].width = 16;
        sprites[s].palette = 0;
        sprites[s].visible = false;
    }
}


/**
 * drawSprites
 * Draws sprites on the pixel buffer.
 * Special thanks to Adam Green for helping me optimize this!
 * @param screenWidth width of screen in pixels
 * @param screenHeight height of screen in pixels
 * @param raster_y current raster Y position
 * @param pixels pixel buffer (scanline)
 */
void
__no_inline_not_in_flash_func(drawSprites)(uint16_t screenWidth, uint16_t screenHeight, uint16_t raster_y, uint16_t pixels[screenWidth]) {
    if (raster_y >= screenHeight) return;

    for (int s = 0; s < NUMBER_OF_SPRITES; s++) {
        if (!sprites[s].visible) continue;
        if (raster_y >= sprites[s].y && raster_y < (sprites[s].y + sprites[s].height)) {
            int offset = (raster_y - sprites[s].y);
            uint32_t line = sprites[s].frame[offset];

            // Pre-calculate the highest pixel address (rightmost) in pixels.
            // Can just decrement this pointer rather than recalculating for each of the 16 pixels.
            uint16_t *pixel = &pixels[sprites[s].x + 15];  // Right most sprite pixel (+15)

            // Pre-calculate the palette which is appropriate for this sprite.
            // Previously it would be recalculated for each of the 16 pixels.
            SpritePalette *palette = &sprite_palettes[sprites[s].palette];

            // Perform the color extraction from line and the pixel update for each pixel at the same time to
            // reduce register pressure. This saves extra memory reads/writes to recover pixel color values from the
            // stack.
            // I updated them in reverse order to simplify the calculation down to right shift by 2-bits and masking off
            // the lower 2-bits (same operation for each pixel).
            uint8_t cf = line & 0b11;
            if (cf) { *pixel = palette->color[cf]; }

            line >>= 2;
            pixel--;
            uint8_t ce = line & 0b11;
            if (ce) { *pixel = palette->color[ce]; }

            line >>= 2;
            pixel--;
            uint8_t cd = line & 0b11;
            if (cd) { *pixel = palette->color[cd]; }

            line >>= 2;
            pixel--;
            uint8_t cc = line & 0b11;
            if (cc) { *pixel = palette->color[cc]; }

            line >>= 2;
            pixel--;
            uint8_t cb = line & 0b11;
            if (cb) { *pixel = palette->color[cb]; }

            line >>= 2;
            pixel--;
            uint8_t ca = line & 0b11;
            if (ca) { *pixel = palette->color[ca]; }

            line >>= 2;
            pixel--;
            uint8_t c9 = line & 0b11;
            if (c9) { *pixel = palette->color[c9]; }

            line >>= 2;
            pixel--;
            uint8_t c8 = line & 0b11;
            if (c8) { *pixel = palette->color[c8]; }

            line >>= 2;
            pixel--;
            uint8_t c7 = line & 0b11;
            if (c7) { *pixel = palette->color[c7]; }

            line >>= 2;
            pixel--;
            uint8_t c6 = line & 0b11;
            if (c6) { *pixel = palette->color[c6]; }

            line >>= 2;
            pixel--;
            uint8_t c5 = line & 0b11;
            if (c5) { *pixel = palette->color[c5]; }

            line >>= 2;
            pixel--;
            uint8_t c4 = line & 0b11;
            if (c4) { *pixel = palette->color[c4]; }

            line >>= 2;
            pixel--;
            uint8_t c3 = line & 0b11;
            if (c3) { *pixel = palette->color[c3]; }

            line >>= 2;
            pixel--;
            uint8_t c2 = line & 0b11;
            if (c2) { *pixel = palette->color[c2]; }

            line >>= 2;
            pixel--;
            uint8_t c1 = line & 0b11;
            if (c1) { *pixel = palette->color[c1]; }

            line >>= 2;
            pixel--;
            uint8_t c0 = line & 0b11;
            if (c0) { *pixel = palette->color[c0]; }
        }
    }
}


void updateSprites() {
    for (int s = 0; s < NUMBER_OF_SPRITES; s++) {
        sprites[s].x = sprites[s].x + sprites[s].x_speed;
        sprites[s].y = sprites[s].y + sprites[s].y_speed;

        if (sprites[s].x > vgaParams.vga_virtual_pixel_width + 64 - 16) sprites[s].x = 0;
        if (sprites[s].x < 0) sprites[s].x = (vgaParams.vga_virtual_pixel_width + 64 - 16); // buffer padding - sprite width
    }
}

void setSprite(uint8_t number, int x, int y, int x_speed, int y_speed) {
    if (number < 0 || number >= NUMBER_OF_SPRITES) return;
    sprites[number].x = x;
    sprites[number].y = y;
    sprites[number].x_speed = x_speed;
    sprites[number].y_speed = y_speed;
}

void setSpriteFrame(uint8_t number, uint32_t *frame) {
    if (number < 0 || number >= NUMBER_OF_SPRITES) return;
    sprites[number].frame = frame;
}

void setSpriteSize(uint8_t number, uint8_t width, uint8_t height) {
    if (number < 0 || number >= NUMBER_OF_SPRITES) return;
    sprites[number].width = 16;     // This is manually set to 16 no matter what.  In the future, this might change.

    if (height < 0 || height >= VGA_VIRTUAL_HEIGHT) height = 16;  // some common sense restraints
    sprites[number].height = height;
}

void setSpritePalette(uint8_t number, uint8_t palette_number) {
    if (number < 0 || number >= NUMBER_OF_SPRITES) return;
    if (palette_number < 0 || palette_number >= NUMBER_OF_SPRITE_PALETTES) palette_number = 0;
    sprites[number].palette = palette_number;
}

void setSpritePosition(uint8_t number, int x, int y) {
    if (number < 0 || number >= NUMBER_OF_SPRITES) return;
    sprites[number].x = x;
    sprites[number].y = y;
//    if (sprites[number].x < 0) sprites[number].x = vgaParams.vga_virtual_pixel_width + (BUFFER_PADDING - 32 - 1);
//    if (sprites[number].x > vgaParams.vga_virtual_pixel_width + (BUFFER_PADDING - 32)) sprites[number].x = 0;
}

void setSpriteSpeed(uint8_t number, int x_speed, int y_speed) {
    if (number < 0 || number >= NUMBER_OF_SPRITES) return;
    sprites[number].x_speed = x_speed;
    sprites[number].y_speed = y_speed;
}


void setSpriteVisible(uint8_t number, bool visible) {
    if (number < 0 || number >= NUMBER_OF_SPRITES) return;
    sprites[number].visible = visible;
}
