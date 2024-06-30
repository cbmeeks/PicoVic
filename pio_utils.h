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

#ifndef _PICOVIC_PIO_UTILS_H
#define _PICOVIC_PIO_UTILS_H

#include "hardware/pio.h"

void pio_set_y(PIO pio, uint sm, uint32_t y);

#endif