#include "screen_modes.h"

VgaParams getScreenModeParams(ScreenMode mode) {
    VgaParams params;

    switch (mode) {
        /**
         *  Name:                   Screen Mode 0
         *  Actual Resolution:      640 x 480 @ 60Hz (from http://tinyvga.com/vga-timing/640x480@60Hz)
         *  Virtual Resolution:     320 x 240
         *  Character Resolution:   40 x 30
         */
        case VGA_640x480_60Hz:
            params.vga_virtual_pixel_width = 320;
            params.vga_virtual_pixel_height = 240;

            params.pixel_clock_in_khz = 25175.0f;
            params.active_x_pixels = 640;
            params.front_porch_x_pixels = 16;
            params.hsync_x_pixels = 96;
            params.back_porch_x_pixels = 48;
            params.hsync_pulse_positive = 0;

            params.active_y_pixels = 480;
            params.front_porch_y_pixels = 10;
            params.vsync_y_pixels = 2;
            params.back_porch_y_pixels = 33;
            params.vsync_pulse_positive = 0;

            params.pixel_scale_x = ((float) params.active_x_pixels / (float) params.vga_virtual_pixel_width);
            params.pixel_scale_y = ((float) params.active_x_pixels / (float) params.vga_virtual_pixel_height);

            if (params.pixel_scale_x < 1) params.pixel_scale_x = 1;
            if (params.pixel_scale_y < 1) params.pixel_scale_y = 1;

            params.total_x_pixels = (
                    params.active_x_pixels +
                    params.front_porch_x_pixels +
                    params.hsync_x_pixels +
                    params.back_porch_x_pixels
            );
            params.total_y_pixels = (
                    params.active_y_pixels +
                    params.front_porch_y_pixels +
                    params.vsync_y_pixels +
                    params.back_porch_y_pixels
            );
            break;
    }

    return params;
}