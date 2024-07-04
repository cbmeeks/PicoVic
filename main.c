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


void endOfFrameCallback(uint64_t frame_counter) {
//    drawCharacterString("*");
}

int main(void) {
    set_sys_clock_khz(252000, false);

    VgaInitParams params = {0};
    params.scanlineFn = screen_Mode1_Scanline;
    params.endOfFrameFn = endOfFrameCallback;

    vgaInit(params, getScreenModeParams(SCREEN_MODE_0));
    initCharMode();
    initSprites();

//    setTextCursor(0, 20);
//    drawCharacterString("320X200 PIXELS...");

//    uint8_t c = 0;
//    for (int y = 0; y < TEXT_MODE_HEIGHT; y++) {
//        for (int x = 0; x < TEXT_MODE_WIDTH; x++) {
//            drawCharacter(x, y, c % 255);
//            c++;
//        }
//    }

    for (int s = 0; s < 20; s++) {
        setSpriteFrame(s, zoomer);
        setSpriteVisible(s, true);
        setSprite(s, s * 16, 0, 0, (rand() % 4));
        setSpriteHeight(s, 32);
    }

    for (int s = 20; s < 40; s++) {
        setSpriteFrame(s, zoomer);
        setSpriteVisible(s, true);
        setSprite(s, s * 16, 64, (rand() % 2), (rand() % 4));
        setSpriteHeight(s, 32);
    }

    setSprite(41, 304, 240 - 32, 0, 0);
    setSpriteFrame(41, samus);
    setSpriteVisible(41, true);

    setSprite(42, 0, 0, 0, 0);
    setSpriteFrame(42, blank);
    setSpriteVisible(42, true);

    setPalette(0, 0x0000);
    setPalette(1, 0x0400);



    while (1) {
        sleep_ms(60);
        drawCharacterString("ABC\t");

        tight_loop_contents();
    }

    return 0;
}
