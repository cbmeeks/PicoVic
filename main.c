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

    uint16_t x = 0;
    for (int s = 0; s < 20; s++) {
        setSpriteFrame(s, zoomer);
        setSpriteVisible(s, true);
        setSprite(s, x, 0, 0, 1);
        setSpriteHeight(s, 32);
        setSpriteVisible(s, false);
        x += 16;
    }

    x = 0;
    for (int s = 20; s < 40; s++) {
        setSpriteFrame(s, zoomer);
        setSpriteVisible(s, true);
        setSprite(s, x, 32, 0, 1);
        setSpriteHeight(s, 32);
        setSpriteVisible(s, false);
        x += 16;
    }

    x = 0;
    for (int s = 40; s < 60; s++) {
        setSpriteFrame(s, zoomer);
        setSpriteVisible(s, true);
        setSprite(s, x, 64, 0, 2);
        setSpriteHeight(s, 32);
        setSpriteVisible(s, false);
        x += 16;
    }

    setSprite(60, 200, 240 - 16 - 48, 0, 0);
    setSpriteFrame(60, samus);
    setSpriteHeight(60, 48);
    setSpriteVisible(60, true);
    setSpritePalette(60, 1);

    setPalette(0, 0x0000);
    setPalette(1, 0x0d28);


    while (1) {
        tight_loop_contents();

        sleep_ms(10);
        drawCharacterString("SAMUS ARAN\\");

    }

    return 0;
}
