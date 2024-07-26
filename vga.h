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

#pragma once

#include <inttypes.h>
#include "screen_modes.h"
#include "chars.h"

#define VGA_VIRTUAL_WIDTH  320
#define VGA_VIRTUAL_HEIGHT 240

#define swap(a, b) { short t = a; a = b; b = t; }

extern void scanline_chars(uint16_t raster_y, uint16_t pixels[VGA_VIRTUAL_WIDTH]);
extern void scanline_chars_sprites(uint16_t raster_y, uint16_t pixels[VGA_VIRTUAL_WIDTH]);
extern void scanline_bitmap_sprites(uint16_t raster_y, uint16_t pixels[VGA_VIRTUAL_WIDTH]);
extern void scanline_map_sprites(uint16_t raster_y, uint16_t pixels[VGA_VIRTUAL_WIDTH]);

typedef void (*vgaScanlineFn)(uint16_t y, uint16_t pixels[VGA_VIRTUAL_WIDTH]);
typedef void (*vgaEndOfFrameFn)(uint64_t frame_counter);
typedef void (*vgaEndOfScanlineFn)();

typedef struct {
    vgaScanlineFn scanlineFn;
    vgaEndOfFrameFn endOfFrameFn;
    vgaEndOfScanlineFn endOfScanlineFn;
} VgaInitParams;


// Setup
void vgaInit(VgaInitParams params, VgaParams modeParams);

// Pixels
void drawPixel(uint16_t x, uint16_t y, uint16_t color);
void drawVLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color);
void drawHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color);
void drawLine(short x0, short y0, short x1, short y1, uint16_t color);
void drawRect(short x, short y, short w, short h, uint16_t color);
void drawRectCenter(short x, short y, short w, short h, uint16_t color);
void drawCircle(short x0, short y0, short r, uint16_t color);
void fillCircle(short x0, short y0, short r, uint16_t color);
void fillRect(short x, short y, short w, short h, uint16_t color);

