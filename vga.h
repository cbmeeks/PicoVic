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

#ifndef _PICOVIC_VGA_H
#define _PICOVIC_VGA_H

#include <inttypes.h>

#define VGA_VIRTUAL_WIDTH  320
#define VGA_VIRTUAL_HEIGHT 240

#define TEXT_MODE_WIDTH 40
#define TEXT_MODE_HEIGHT 30

extern uint16_t palette_buffer[];
extern uint16_t frame_buffer[];
extern uint8_t text_buffer[];
extern uint8_t text_fg_clut[];
extern uint8_t text_bg_clut[];


extern void screen_Mode0_Scanline(uint16_t raster_y, uint16_t pixels[320]);

typedef void (*vgaScanlineFn)(uint16_t y, uint16_t pixels[VGA_VIRTUAL_WIDTH]);

typedef struct {
    vgaScanlineFn scanlineFn;
} VgaInitParams;


void initPalette();

void setTextCursor(uint8_t x, uint8_t y);

void setPalette(uint8_t paletteNumber, uint16_t color);

void setFGColor(uint8_t x, uint8_t y, uint8_t paletteNumber);

void setBGColor(uint8_t x, uint8_t y, uint8_t paletteNumber);


void vgaInit(VgaInitParams params);

void initCharMode();

void drawCharacter(uint8_t x, uint8_t y, uint8_t character);

void drawAsciiCharacter(uint8_t x, uint8_t y, uint8_t character);

void drawCharacterString(char *str);

void text_write(unsigned char c);

void shiftCharactersUp();


void drawPixel(uint16_t x, uint16_t y, uint16_t color);

void drawVLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color);

void drawHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color);


#endif