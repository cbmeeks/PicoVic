/*
 * Project: PicoVic - vga
 *
 * Copyright (c) 2024 Cecil Meeks
 * https://github.com/cbmeeks/PicoVic
 *
 *
 * Basic VGA code taken from pico-56 - vga by Troy Schrapel
 * Copyright (c) 2023 Troy Schrapel
 * https://github.com/visrealm/pico-56
 *
 * This code is licensed under the MIT license
 *
 */


#include <stdlib.h>
#include "vga.h"
#include "screen_modes.h"
#include "pico/stdlib.h"
#include "res/sprites/sprite_defs.h"
#include "tiles.h"
#include "sprites.h"


void endOfFrameCallback(uint64_t frame_counter) {
//    drawCharacterString("*");
}

int main(void) {
    set_sys_clock_khz(252000, false);

    VgaInitParams params = {0};
    params.scanlineFn = screen_Mode3_Scanline;
    params.endOfFrameFn = endOfFrameCallback;

    ScreenModeParams screenModeParams = getScreenModeParams(SCREEN_MODE_0);

    vgaInit(params, screenModeParams);
    initCharMode();
    initMap();
    initSprites(screenModeParams);

    uint8_t sprites_per_scanline = 12;

    int p = 0;
    uint16_t x = 16;
    uint8_t y = 16;
    for (int s = 0; s < sprites_per_scanline - 1; s++) {
        setSpriteFrame(s, zoomer);
        setSprite(s, x, y, 0, 0);
        setSpriteSize(s, 16, 16);
        setSpriteVisible(s, true);
        setSpritePalette(s, 4);
        x += 16;
    }

    x = 16;
    y += 20;
    for (int s = 16; s < 16 + sprites_per_scanline - 1; s++) {
        setSpriteFrame(s, blank16x16);
        setSprite(s, x, y, 1, 0);
        setSpriteSize(s, 16, 16);
        setSpriteVisible(s, true);
        setSpritePalette(s, 4);
        x += 16;
    }

    x = 16;
    y += 60;
    for (int s = 32; s < 32 + sprites_per_scanline - 1; s++) {
        setSpriteFrame(s, skrees[0]);
        setSprite(s, x, y, -1, 0);
        setSpriteSize(s, 16, 24);
        setSpriteVisible(s, true);
        setSpritePalette(s, 4);
        x += 16;
    }

    setSprite(32, 160, 0, 0, 2);


    x = 112;
    y += 20;
    for (int s = 48; s < 48 + sprites_per_scanline - 1; s++) {
        setSpriteFrame(s, samus);
        setSprite(s, x, 240 - 16 - 48, 0, 0);
        setSpriteSize(s, 16, 48);
        setSpriteVisible(s, true);
        setSpritePalette(s, 5);
        x += 16;
    }
    setSpritePosition(52, 11 * 16, 240 - 16 - 48 - 32);


    setPalette(0, 0x0000);
    setPalette(1, 0x0d28);


    uint8_t frame = 0;
    while (1) {
        tight_loop_contents();

        sleep_ms(100);
        setSpriteFrame(32, skrees[frame++]);
        if (frame >= 3) frame = 0;

//        drawCharacterString("SAMUS ARAN\\");

    }

    return 0;
}
