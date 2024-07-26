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

#include "sprites.h"
#include "../res/sprites/sprite_defs.h"

void endOfFrameCallback(uint64_t frame_counter) {
//    drawCharacterString("*");
}

int main(void) {
    set_sys_clock_khz(252000, false);

    VgaInitParams vgaInitParams = {0};
    vgaInitParams.scanlineFn = scanline_map_sprites;
    vgaInitParams.endOfFrameFn = endOfFrameCallback;

    VgaParams vgaParams = getScreenModeParams(VGA_640x480_60Hz);

    vgaInit(vgaInitParams, vgaParams);
    initCharMode();
    initSprites(vgaParams);


    int sprites_per_scanline = 24;
    for (int i = 0; i < sprites_per_scanline; i++) {
        setSprite(i, 1 + (i * 8), 64, 1, 0);
        setSpriteFrame(i, blank16x16);
        setSpriteSize(i, 16, 192);
        setSpriteVisible(i, true);
        setSpritePalette(i, i % 6);
    }
//    setSpriteFrame(11, samus);
//    setSpriteSize(11, 16, 48);
//    setSpriteSpeed(11, 1, 0);
//    setSpritePalette(11, 6);
//
//    setSpriteFrame(1, samus);
//    setSpriteSize(1, 16, 48);
//    setSpriteFrame(3, samus);
//    setSpriteSize(3, 16, 48);
//    setSpriteFrame(5, samus);
//    setSpriteSize(5, 16, 48);
//    setSpriteFrame(7, samus);
//    setSpriteSize(7, 16, 48);
//    setSpriteFrame(10, samus);
//    setSpriteSize(10, 16, 48);


    setTextCursor(0, 0);
    setPalette(0, 0x0000);
    setPalette(1, 0x0a00);

    uint16_t color = 0;
    while (1) {
        tight_loop_contents();
        drawCharacterString("HELLO WORLD! ");

//        fillRect(rand() % 320, rand() % 240, rand() % 64, rand() % 64, rand() % 4096);
        sleep_ms(64);
    }

    return 0;
}
