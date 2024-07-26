#include <sys/cdefs.h>
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
#include "vga.pio.h"
#include "pio_utils.h"

#include "pico/divider.h"
#include "pico/multicore.h"

#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

#include "bit_helper.h"
#include "screen_modes.h"

#include "sprites.h"
#include "tiles.h"

#include <math.h>
#include <string.h>
#include <stdlib.h>


#define SYNC_PINS_START 0        // first sync pin gpio number
#define SYNC_PINS_COUNT 2        // number of sync pins (h and v)

#define RGB_PINS_START  2        // first rgb pin gpio number
#define RGB_PINS_COUNT 12        // number of rgb pins

#define VGA_PIO         pio0_hw  // which pio are we using for vga?
#define SYNC_SM         0        // vga sync state machine index
#define RGB_SM          1        // vga rgb state machine index

#define END_OF_SCANLINE_MSG     0x40000000
#define END_OF_FRAME_MSG        0x80000000

// Private methods
void drawCircleHelper(short x0, short y0, short r, uint16_t cornername, uint16_t color);
void fillCircleHelper(short x0, short y0, short r, uint16_t cornername, short delta, uint16_t color);


/*
 * sync pio dma data buffers
 */
uint32_t __aligned(4) syncDataActive[4];  // active display area
uint32_t __aligned(4) syncDataPorch[4];   // vertical porch
uint32_t __aligned(4) syncDataSync[4];    // vertical sync

uint16_t __aligned(4) rgbDataBufferA[VGA_VIRTUAL_WIDTH];
uint16_t __aligned(4) rgbDataBufferB[VGA_VIRTUAL_WIDTH];

#define SYNC_LINE_ACTIVE 0
#define SYNC_LINE_FPORCH 1
#define SYNC_LINE_HSYNC  2
#define SYNC_LINE_BPORCH 3


/**
 * FRAME BUFFER
 * This frame buffer can be used as a "bitmap" mode that supports basic drawing.
 */
#define PIXELCOUNT (VGA_VIRTUAL_WIDTH * VGA_VIRTUAL_HEIGHT)
uint16_t __aligned(4) frame_buffer[VGA_VIRTUAL_WIDTH * VGA_VIRTUAL_HEIGHT];


/*
 * file scope
 */
static int syncDmaChan = 0;
static int rgbDmaChan = 0;
static VgaInitParams vgaInitParams;
static VgaParams vgaParams;


/*
 * build the sync data buffers
 */
static void buildSyncData(void) {
    const uint32_t sysClockKHz = clock_get_hz(clk_sys) / 1000;
    const uint32_t pioClockKHz = sysClockKHz;

    // compute number of pio ticks for a single pixel
    const float pioClocksPerPixel = (float) pioClockKHz / vgaParams.pixel_clock_in_khz;

    // compute pio ticks for each phase of the scanline
    const uint32_t activeTicks = round(pioClocksPerPixel * (float) vgaParams.active_x_pixels) - vga_sync_SETUP_OVERHEAD;
    const uint32_t fPorchTicks = round(pioClocksPerPixel * (float) vgaParams.front_porch_x_pixels) - vga_sync_SETUP_OVERHEAD;
    const uint32_t syncTicks = round(pioClocksPerPixel * (float) vgaParams.hsync_x_pixels) - vga_sync_SETUP_OVERHEAD;
    const uint32_t bPorchTicks = round(pioClocksPerPixel * (float) vgaParams.back_porch_x_pixels) - vga_sync_SETUP_OVERHEAD;

    // compute sync bits
    const uint32_t hSyncOff = (vgaParams.hsync_pulse_positive ? 0b00 : 0b01) << vga_sync_WORD_SYNC_OFFSET;
    const uint32_t hSyncOn = (vgaParams.hsync_pulse_positive ? 0b01 : 0b00) << vga_sync_WORD_SYNC_OFFSET;
    const uint32_t vSyncOff = (vgaParams.vsync_pulse_positive ? 0b00 : 0b10) << vga_sync_WORD_SYNC_OFFSET;
    const uint32_t vSyncOn = (vgaParams.vsync_pulse_positive ? 0b10 : 0b00) << vga_sync_WORD_SYNC_OFFSET;

    // compute exec instructions
    const uint32_t instIrq = pio_encode_irq_set(false, vga_rgb_RGB_IRQ) << vga_sync_WORD_EXEC_OFFSET;
    const uint32_t instNop = pio_encode_nop() << vga_sync_WORD_EXEC_OFFSET;

    // sync data for an active display scanline
    syncDataActive[SYNC_LINE_ACTIVE] = instIrq | vSyncOff | hSyncOff | activeTicks;
    syncDataActive[SYNC_LINE_FPORCH] = instNop | vSyncOff | hSyncOff | fPorchTicks;
    syncDataActive[SYNC_LINE_HSYNC] = instNop | vSyncOff | hSyncOn | syncTicks;
    syncDataActive[SYNC_LINE_BPORCH] = instNop | vSyncOff | hSyncOff | bPorchTicks;

    // sync data for a front or back porch scanline
    syncDataPorch[SYNC_LINE_ACTIVE] = instNop | vSyncOff | hSyncOff | activeTicks;
    syncDataPorch[SYNC_LINE_FPORCH] = instNop | vSyncOff | hSyncOff | fPorchTicks;
    syncDataPorch[SYNC_LINE_HSYNC] = instNop | vSyncOff | hSyncOn | syncTicks;
    syncDataPorch[SYNC_LINE_BPORCH] = instNop | vSyncOff | hSyncOff | bPorchTicks;

    // sync data for a vsync scanline
    syncDataSync[SYNC_LINE_ACTIVE] = instNop | vSyncOn | hSyncOff | activeTicks;
    syncDataSync[SYNC_LINE_FPORCH] = instNop | vSyncOn | hSyncOff | fPorchTicks;
    syncDataSync[SYNC_LINE_HSYNC] = instNop | vSyncOn | hSyncOn | syncTicks;
    syncDataSync[SYNC_LINE_BPORCH] = instNop | vSyncOn | hSyncOff | bPorchTicks;
}


/*
 * initialise the vga sync pio
 */
static void vgaInitSync(void) {
    buildSyncData();

    // initialize sync pins for pio
    for (uint i = 0; i < SYNC_PINS_COUNT; ++i) {
        pio_gpio_init(VGA_PIO, SYNC_PINS_START + i);
    }

    // add sync pio program
    uint syncProgOffset = pio_add_program(VGA_PIO, &vga_sync_program);
    pio_sm_set_consecutive_pindirs(VGA_PIO, SYNC_SM, SYNC_PINS_START, SYNC_PINS_COUNT, true);

    // configure sync pio
    pio_sm_config syncConfig = vga_sync_program_get_default_config(syncProgOffset);
    sm_config_set_out_pins(&syncConfig, SYNC_PINS_START, 2);
    sm_config_set_clkdiv(&syncConfig, 1);
    sm_config_set_out_shift(&syncConfig, true, true, 32); // R shift, autopull @ 32 bits
    sm_config_set_fifo_join(&syncConfig, PIO_FIFO_JOIN_TX); // Join FIFOs together to get an 8 entry TX FIFO
    pio_sm_init(VGA_PIO, SYNC_SM, syncProgOffset, &syncConfig);

    // initialise sync dma
    syncDmaChan = dma_claim_unused_channel(true);
    dma_channel_config syncDmaChanConfig = dma_channel_get_default_config(syncDmaChan);
    channel_config_set_transfer_data_size(&syncDmaChanConfig, DMA_SIZE_32);           // transfer 32 bits at a time
    channel_config_set_read_increment(&syncDmaChanConfig, true);                       // increment read
    channel_config_set_write_increment(&syncDmaChanConfig, false);                     // don't increment write
    channel_config_set_dreq(&syncDmaChanConfig, pio_get_dreq(VGA_PIO, SYNC_SM, true)); // transfer when there's space in fifo

    // setup the dma channel and set it going
    dma_channel_configure(syncDmaChan, &syncDmaChanConfig, &VGA_PIO->txf[SYNC_SM], syncDataSync, 4, false);
    dma_channel_set_irq0_enabled(syncDmaChan, true);
}


/*
 * initialise the vga sync pio
 */
static void vgaInitRgb(void) {
    const uint32_t sysClockKHz = clock_get_hz(clk_sys) / 1000;
    uint32_t pioClockKHz = sysClockKHz;
    float pioClkDiv = 1.0f;
    float pioClocksPerScaledPixel = pioClockKHz * vgaParams.pixel_scale_x / vgaParams.pixel_clock_in_khz;
    while (pioClocksPerScaledPixel > 31) {
        ++pioClkDiv;
        pioClockKHz = sysClockKHz / pioClkDiv;
        pioClocksPerScaledPixel = pioClockKHz * vgaParams.pixel_scale_x / vgaParams.pixel_clock_in_khz;
    }

    // compute number of pio ticks for a single pixel
    const float pioClocksPerPixel = pioClockKHz / vgaParams.pixel_clock_in_khz;
    const uint32_t rgbCyclesPerPixel = round(pioClocksPerScaledPixel);

    // copy the rgb program and set the appropriate pixel delay
    uint16_t rgbProgramInstr[vga_rgb_program.length];
    memcpy(rgbProgramInstr, vga_rgb_program.instructions, sizeof(rgbProgramInstr));
    rgbProgramInstr[vga_rgb_DELAY_INSTR] |= pio_encode_delay(rgbCyclesPerPixel - vga_rgb_LOOP_TICKS);

    pio_program_t rgbProgram = {
            .instructions = rgbProgramInstr,
            .length = vga_rgb_program.length,
            .origin = vga_rgb_program.origin
    };

    // initialize sync pins for pio
    for (uint i = 0; i < RGB_PINS_COUNT; ++i) {
        pio_gpio_init(VGA_PIO, RGB_PINS_START + i);
    }

    // add rgb pio program
    pio_sm_set_consecutive_pindirs(VGA_PIO, RGB_SM, RGB_PINS_START, RGB_PINS_COUNT, true);
    pio_set_y(VGA_PIO, RGB_SM, vgaParams.vga_virtual_pixel_width - 1);

    uint rgbProgOffset = pio_add_program(VGA_PIO, &rgbProgram);
    pio_sm_config rgbConfig = vga_rgb_program_get_default_config(rgbProgOffset);

    sm_config_set_out_pins(&rgbConfig, RGB_PINS_START, RGB_PINS_COUNT);
    sm_config_set_clkdiv(&rgbConfig, pioClkDiv);

    sm_config_set_fifo_join(&rgbConfig, PIO_FIFO_JOIN_TX);

    sm_config_set_out_shift(&rgbConfig, true, true, 16); // R shift, autopull @ 16 bits
    pio_sm_init(VGA_PIO, RGB_SM, rgbProgOffset, &rgbConfig);

    // initialise rgb dma
    rgbDmaChan = dma_claim_unused_channel(true);
    dma_channel_config rgbDmaChanConfig = dma_channel_get_default_config(rgbDmaChan);
    channel_config_set_transfer_data_size(&rgbDmaChanConfig, DMA_SIZE_16);  // transfer 16 bits at a time
    channel_config_set_read_increment(&rgbDmaChanConfig, true);             // increment read
    channel_config_set_write_increment(&rgbDmaChanConfig, false);           // don't increment write
    channel_config_set_dreq(&rgbDmaChanConfig, pio_get_dreq(VGA_PIO, RGB_SM, true));

    // setup the dma channel and set it going
    dma_channel_configure(rgbDmaChan,
                          &rgbDmaChanConfig,
                          &VGA_PIO->txf[RGB_SM],
                          rgbDataBufferA,
                          vgaParams.vga_virtual_pixel_width,
                          false
    );
    dma_channel_set_irq0_enabled(rgbDmaChan, true);
}

/*
 * DMA interrupt handler
 */
static void __time_critical_func(dmaIrqHandler)(void) {
    static int currentTimingLine = -1;
    static int currentDisplayLine = -1;

    if (dma_hw->ints0 & (1u << syncDmaChan)) {
        dma_hw->ints0 = 1u << syncDmaChan;

        if (++currentTimingLine >= vgaParams.total_y_pixels) {
            currentTimingLine = 0;
            currentDisplayLine = 0;
        }

        if (currentTimingLine < vgaParams.vsync_y_pixels) {
            dma_channel_set_read_addr(syncDmaChan, syncDataSync, true);
        } else if (currentTimingLine < (vgaParams.vsync_y_pixels + vgaParams.back_porch_y_pixels)) {
            dma_channel_set_read_addr(syncDmaChan, syncDataPorch, true);
        } else if (currentTimingLine < (vgaParams.total_y_pixels - vgaParams.front_porch_y_pixels)) {
            dma_channel_set_read_addr(syncDmaChan, syncDataActive, true);
        } else {
            dma_channel_set_read_addr(syncDmaChan, syncDataPorch, true);
        }
    }

    if (dma_hw->ints0 & (1u << rgbDmaChan)) {
        dma_hw->ints0 = 1u << rgbDmaChan;

        divmod_result_t pxLineVal = divmod_u32u32(++currentDisplayLine, vgaParams.pixel_scale_y);
        uint32_t pxLine = to_quotient_u32(pxLineVal);
        uint32_t pxLineRpt = to_remainder_u32(pxLineVal);

        dma_channel_set_read_addr(rgbDmaChan, (pxLine & 1) ? rgbDataBufferB : rgbDataBufferA, true);

        // need a new line every X display lines
        if (pxLineRpt == 0) {
            uint32_t requestLine = pxLine + 1;
            if (requestLine >= VGA_VIRTUAL_HEIGHT) requestLine -= VGA_VIRTUAL_HEIGHT;

            multicore_fifo_push_timeout_us(requestLine, 0);
        }
    }
}

/*
 * initialise the pio dma
 */
static void initDma() {
    irq_set_exclusive_handler(DMA_IRQ_0, dmaIrqHandler);
    irq_set_enabled(DMA_IRQ_0, true);

    dma_channel_start(syncDmaChan);
    dma_channel_start(rgbDmaChan);
}


/*
 * Main VGA Loop
 * Will execute "end of frame" and "end of line" callbacks if they are available.
 */
_Noreturn static void vgaLoop() {
    uint64_t frame_counter = 0;

    while (1) {
        uint32_t message = multicore_fifo_pop_blocking();
        if (message == END_OF_FRAME_MSG) {
            if (vgaInitParams.endOfFrameFn) vgaInitParams.endOfFrameFn(frame_counter);
            ++frame_counter;
        } else if ((message & END_OF_SCANLINE_MSG) != 0) {
            if (vgaInitParams.endOfScanlineFn) vgaInitParams.endOfScanlineFn();
        } else if (message & 0x01) {
            vgaInitParams.scanlineFn(message & 0xfff, rgbDataBufferB);
        } else {
            vgaInitParams.scanlineFn(message & 0xfff, rgbDataBufferA);
        }
    }
}


/*
 * initialise the vga
 */
void vgaInit(VgaInitParams params, VgaParams modeParams) {
    vgaInitParams = params;
    vgaParams = modeParams;

    vgaInitSync();
    vgaInitRgb();

    initDma();

    pio_sm_set_enabled(VGA_PIO, SYNC_SM, true);
    pio_sm_set_enabled(VGA_PIO, RGB_SM, true);

    multicore_launch_core1(vgaLoop);
}


void scanline_chars(uint16_t raster_y, uint16_t pixels[vgaParams.vga_virtual_pixel_width]) {
    drawChars(raster_y, vgaParams.vga_virtual_pixel_height, pixels);
}

void scanline_chars_sprites(uint16_t raster_y, uint16_t pixels[vgaParams.vga_virtual_pixel_width]) {
    uint16_t screenWidth = vgaParams.vga_virtual_pixel_width;
    uint16_t screenHeight = vgaParams.vga_virtual_pixel_height;

    drawChars(raster_y, screenHeight, pixels);
    drawSprites(screenWidth, screenHeight, raster_y, pixels);

    // About 60 fps
    if (raster_y == vgaParams.vga_virtual_pixel_height - 1) {
        updateSprites();
    }
}

// Bitmap line with sprites
void scanline_bitmap_sprites(uint16_t raster_y, uint16_t pixels[vgaParams.vga_virtual_pixel_width]) {
    uint16_t screenWidth = vgaParams.vga_virtual_pixel_width;
    uint16_t screenHeight = vgaParams.vga_virtual_pixel_height;

    memcpy(pixels, frame_buffer + raster_y * screenWidth, screenWidth * sizeof(uint16_t));
    drawSprites(screenWidth, screenHeight, raster_y, pixels);

    if (raster_y == vgaParams.vga_virtual_pixel_height - 1) {
        updateSprites();
    }
}

void scanline_map_sprites(uint16_t raster_y, uint16_t pixels[vgaParams.vga_virtual_pixel_width]) {
    uint16_t screenWidth = vgaParams.vga_virtual_pixel_width;
    uint16_t screenHeight = vgaParams.vga_virtual_pixel_height;

    drawMap(screenWidth, screenHeight, raster_y, pixels);
    drawSprites(screenWidth, screenHeight, raster_y, pixels);

    // About 60 fps
    if (raster_y == vgaParams.vga_virtual_pixel_height - 1) {
        updateMap();
        updateSprites();
    }
}


/**
 * A function for drawing a pixel with a specified color.
 * Note that because information is passed to the PIO state machines through a DMA channel,
 * we only need to modify the contents of the array and the pixels will be automatically updated on the screen.
 *
 * @param x X pixel location
 * @param y Y pixel location
 * @param color color to draw
 */
void drawPixel(uint16_t x, uint16_t y, uint16_t color) {
    x %= vgaParams.vga_virtual_pixel_width;
    y %= VGA_VIRTUAL_HEIGHT;
    frame_buffer[y * vgaParams.vga_virtual_pixel_width + x] = color;
}

void drawVLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color) {
    for (uint16_t i = y; i < y + h; i++) {
        drawPixel(x, i, color);
    }
}

void drawHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color) {
    for (uint16_t i = x; i < x + w; i++) {
        drawPixel(i, y, color);
    }
}

// Bresenham's algorithm - thx wikipedia and thx Bruce!
void drawLine(short x0, short y0, short x1, short y1, uint16_t color) {
    /* Draw a straight line from (x0,y0) to (x1,y1) with given color
      Parameters:
           x0: x-coordinate of starting point of line. The x-coordinate of
               the top-left of the screen is 0. It increases to the right.
           y0: y-coordinate of starting point of line. The y-coordinate of
               the top-left of the screen is 0. It increases to the bottom.
           x1: x-coordinate of ending point of line. The x-coordinate of
               the top-left of the screen is 0. It increases to the right.
           y1: y-coordinate of ending point of line. The y-coordinate of
               the top-left of the screen is 0. It increases to the bottom.
           color: 6-bit color value for line
    */
    short steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        swap(x0, y0);
        swap(x1, y1);
    }

    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }

    short dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    short err = dx / 2;
    short ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0 <= x1; x0++) {
        if (steep) {
            drawPixel(y0, x0, color);
        } else {
            drawPixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

// Draw a rectangle
void drawRect(short x, short y, short w, short h, uint16_t color) {
    /* Draw a rectangle outline with top left vertex (x,y), width w
      and height h at given color
      Parameters:
           x:  x-coordinate of top-left vertex. The x-coordinate of
               the top-left of the screen is 0. It increases to the right.
           y:  y-coordinate of top-left vertex. The y-coordinate of
               the top-left of the screen is 0. It increases to the bottom.
           w:  width of the rectangle
           h:  height of the rectangle
           color:  16-bit color of the rectangle outline
      Returns: Nothing
    */
    drawHLine(x, y, w, color);
    drawHLine(x, y + h - 1, w, color);
    drawVLine(x, y, h, color);
    drawVLine(x + w - 1, y, h, color);
}

void drawRectCenter(short x, short y, short w, short h, uint16_t color) {
    drawRect(x - w / 2, y - h / 2, w, h, color);
}

void drawCircle(short x0, short y0, short r, uint16_t color) {
    /* Draw a circle outline with center (x0,y0) and radius r, with given color
      Parameters:
           x0: x-coordinate of center of circle. The top-left of the screen
               has x-coordinate 0 and increases to the right
           y0: y-coordinate of center of circle. The top-left of the screen
               has y-coordinate 0 and increases to the bottom
           r:  radius of circle
           color: 16-bit color value for the circle. Note that the circle
               isn't filled. So, this is the color of the outline of the circle
      Returns: Nothing
    */
    short f = 1 - r;
    short ddF_x = 1;
    short ddF_y = -2 * r;
    short x = 0;
    short y = r;

    drawPixel(x0, y0 + r, color);
    drawPixel(x0, y0 - r, color);
    drawPixel(x0 + r, y0, color);
    drawPixel(x0 - r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        drawPixel(x0 + x, y0 + y, color);
        drawPixel(x0 - x, y0 + y, color);
        drawPixel(x0 + x, y0 - y, color);
        drawPixel(x0 - x, y0 - y, color);
        drawPixel(x0 + y, y0 + x, color);
        drawPixel(x0 - y, y0 + x, color);
        drawPixel(x0 + y, y0 - x, color);
        drawPixel(x0 - y, y0 - x, color);
    }
}

void drawCircleHelper(short x0, short y0, short r, uint16_t cornername, uint16_t color) {
    // Helper function for drawing circles and circular objects
    short f = 1 - r;
    short ddF_x = 1;
    short ddF_y = -2 * r;
    short x = 0;
    short y = r;

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        if (cornername & 0x4) {
            drawPixel(x0 + x, y0 + y, color);
            drawPixel(x0 + y, y0 + x, color);
        }
        if (cornername & 0x2) {
            drawPixel(x0 + x, y0 - y, color);
            drawPixel(x0 + y, y0 - x, color);
        }
        if (cornername & 0x8) {
            drawPixel(x0 - y, y0 + x, color);
            drawPixel(x0 - x, y0 + y, color);
        }
        if (cornername & 0x1) {
            drawPixel(x0 - y, y0 - x, color);
            drawPixel(x0 - x, y0 - y, color);
        }
    }
}

void fillCircle(short x0, short y0, short r, uint16_t color) {
    /* Draw a filled circle with center (x0,y0) and radius r, with given color
      Parameters:
           x0: x-coordinate of center of circle. The top-left of the screen
               has x-coordinate 0 and increases to the right
           y0: y-coordinate of center of circle. The top-left of the screen
               has y-coordinate 0 and increases to the bottom
           r:  radius of circle
           color: 16-bit color value for the circle
      Returns: Nothing
    */
    drawVLine(x0, y0 - r, 2 * r + 1, color);
    fillCircleHelper(x0, y0, r, 3, 0, color);
}

void fillCircleHelper(short x0, short y0, short r, uint16_t cornername, short delta, uint16_t color) {
    // Helper function for drawing filled circles
    short f = 1 - r;
    short ddF_x = 1;
    short ddF_y = -2 * r;
    short x = 0;
    short y = r;

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        if (cornername & 0x1) {
            drawVLine(x0 + x, y0 - y, 2 * y + 1 + delta, color);
            drawVLine(x0 + y, y0 - x, 2 * x + 1 + delta, color);
        }
        if (cornername & 0x2) {
            drawVLine(x0 - x, y0 - y, 2 * y + 1 + delta, color);
            drawVLine(x0 - y, y0 - x, 2 * x + 1 + delta, color);
        }
    }
}

void fillRect(short x, short y, short w, short h, uint16_t color) {
    /* Draw a filled rectangle with starting top-left vertex (x,y),
       width w and height h with given color
      Parameters:
           x:  x-coordinate of top-left vertex; top left of screen is x=0
                   and x increases to the right
           y:  y-coordinate of top-left vertex; top left of screen is y=0
                   and y increases to the bottom
           w:  width of rectangle
           h:  height of rectangle
           color:  3-bit color value
      Returns:     Nothing
    */

    for (int i = x; i < (x + w); i++) {
        for (int j = y; j < (y + h); j++) {
            drawPixel(i, j, color);
        }
    }
}

