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

#include "vga.h"
#include "screen_modes.h"
#include "pico/stdlib.h"


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

//    setTextCursor(0, 20);
//    drawCharacterString("320X200 PIXELS...");

    uint8_t c = 0;
    for (int y = 0; y < TEXT_MODE_HEIGHT; y++) {
        for (int x = 0; x < TEXT_MODE_WIDTH; x++) {
            drawCharacter(x, y, c % 255);
            c++;
        }
    }

    while (1) {
//        sleep_ms(60);
//        updateSprite();
        tight_loop_contents();
    }

    return 0;
}
