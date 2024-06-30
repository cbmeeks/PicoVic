/*
 * Project: PicoVic - pio
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

#include "pio_utils.h"

/*
 * set a pio state machine y register
 */
void pio_set_y(PIO pio, uint sm, uint32_t y) {
    const uint instr_shift = pio_encode_in(pio_x, 4);
    const uint instr_mov = pio_encode_mov(pio_y, pio_isr);

    for (int i = 0; i < 8; ++i) {
        const uint32_t nibble = (y >> (i * 4)) & 0xf;
        pio_sm_exec(pio, sm, pio_encode_set(pio_x, nibble));
        pio_sm_exec(pio, sm, instr_shift);
    }
    pio_sm_exec(pio, sm, instr_mov);
}