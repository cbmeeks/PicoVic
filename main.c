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
#include "pico/stdlib.h"


int main(void) {
    set_sys_clock_khz(252000, false);

    VgaInitParams params;
    params.scanlineFn = screen_Mode0_Scanline;

    vgaInit(params);
    initCharMode();

    setTextCursor(0, 20);
    setPalette(3, 0x0f0F);
    setFGColor(0, 29, 3);
    setBGColor(5, 29, 3);

    while (1) {
        sleep_ms(2000);
        drawCharacterString("ASDFKJGAKJSDHFGKJAH234871234987");
        tight_loop_contents();
    }

    return 0;
}
