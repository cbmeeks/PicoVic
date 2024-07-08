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


extern uint16_t palette_buffer[];
extern uint16_t frame_buffer[];
extern uint8_t text_buffer[];
extern uint8_t text_fg_clut[];
extern uint8_t text_bg_clut[];

extern void screen_Mode0_Scanline(uint16_t raster_y, uint16_t pixels[VGA_VIRTUAL_WIDTH]);
extern void screen_Mode1_Scanline(uint16_t raster_y, uint16_t pixels[VGA_VIRTUAL_WIDTH]);
extern void screen_Mode2_Scanline(uint16_t raster_y, uint16_t pixels[VGA_VIRTUAL_WIDTH]);
extern void screen_Mode3_Scanline(uint16_t raster_y, uint16_t pixels[VGA_VIRTUAL_WIDTH]);

typedef void (*vgaScanlineFn)(uint16_t y, uint16_t pixels[VGA_VIRTUAL_WIDTH]);
typedef void (*vgaEndOfFrameFn)(uint64_t frame_counter);
typedef void (*vgaEndOfScanlineFn)();

typedef struct {
    vgaScanlineFn scanlineFn;
    vgaEndOfFrameFn endOfFrameFn;
    vgaEndOfScanlineFn endOfScanlineFn;
} VgaInitParams;



// Setup
void vgaInit(VgaInitParams params, ScreenModeParams modeParams);


// Pixels
void drawPixel(uint16_t x, uint16_t y, uint16_t color);
void drawVLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color);
void drawHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color);


