#include <stdint-gcc.h>
#include <stdbool.h>

#pragma once

typedef enum {
    VGA_640x480_60Hz,
} ScreenMode;


/**
 * Screen Mode Definitions
 */
typedef struct {
    uint16_t vga_virtual_pixel_width;   // Virtual width of VGA in pixels
    uint16_t vga_virtual_pixel_height;  // Virtual height of VGA in pixels
    bool hsync_pulse_positive;          // HSYNC_PULSE_POSITIVE
    bool vsync_pulse_positive;          // VSYNC_PULSE_POSITIVE

    float pixel_clock_in_khz;           // Pixel clock (typically 25.175 MHz)
    uint16_t active_x_pixels;           // Number of visible horizontal pixels in the scanline
    uint16_t front_porch_x_pixels;      // Number of horizontal pixels for the front porch
    uint16_t hsync_x_pixels;            // Number of horizontal pixels for the HSYNC
    uint16_t back_porch_x_pixels;       // Number of horizontal pixels for the back porch
    float pixel_scale_x;                // (ACTIVE_PIXELS_X / VGA_VIRTUAL_WIDTH)
    uint16_t total_x_pixels;            // (ACTIVE_PIXELS_X + FPORCH_PIXELS_X + HSYNC_PIXELS_X + BPORCH_PIXELS_X)


    uint16_t active_y_pixels;           // Number of visible vertical pixels in the frame
    uint16_t front_porch_y_pixels;      // Number of vertical pixels for the front porch
    uint16_t vsync_y_pixels;            // Number of vertical VSYNC pixels
    uint16_t back_porch_y_pixels;       // Number of vertical pixels in the back porch
    float pixel_scale_y;                // (ACTIVE_PIXELS_Y / VGA_VIRTUAL_HEIGHT)
    uint16_t total_y_pixels;            // (ACTIVE_PIXELS_Y + FPORCH_PIXELS_Y + VSYNC_PIXELS_Y + BPORCH_PIXELS_Y)
} VgaParams;

VgaParams getScreenModeParams(ScreenMode mode);