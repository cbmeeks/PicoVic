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

#include "res/fonts/petscii/petscii.h"
#include "bit_helper.h"
#include "font_utils.h"
#include "screen_modes.h"

#include "sprites.h"
#include "tiles.h"

#include <math.h>
#include <string.h>


#define SYNC_PINS_START 0        // first sync pin gpio number
#define SYNC_PINS_COUNT 2        // number of sync pins (h and v)

#define RGB_PINS_START  2        // first rgb pin gpio number
#define RGB_PINS_COUNT 12        // number of rgb pins

#define VGA_PIO         pio0_hw  // which pio are we using for vga?
#define SYNC_SM         0        // vga sync state machine index
#define RGB_SM          1        // vga rgb state machine index

#define END_OF_SCANLINE_MSG     0x40000000
#define END_OF_FRAME_MSG        0x80000000

/*
 * sync pio dma data buffers
 */
uint32_t __aligned(4) syncDataActive[4];  // active display area
uint32_t __aligned(4) syncDataPorch[4];   // vertical porch
uint32_t __aligned(4) syncDataSync[4];    // vertical sync

uint16_t __aligned(4) rgbDataBufferEven[VGA_VIRTUAL_WIDTH];
uint16_t __aligned(4) rgbDataBufferOdd[VGA_VIRTUAL_WIDTH];


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

/**
 * PALETTE BUFFER
 */
#define PALETTECOUNT 256
uint16_t palette_buffer[PALETTECOUNT];


/**
 * TEXT BUFFER
 * Character buffer used to hold text and other characters.
 * Similar to the $0400 location on a Commodore 64.
 */
#define TEXT_MODE_COUNT (TEXT_MODE_WIDTH * TEXT_MODE_HEIGHT)
#define BLANK_CHAR 32
#define CHAR_WIDTH 8
#define CHAR_HEIGHT 8

uint8_t cursor_x, cursor_y;
uint8_t text_buffer[TEXT_MODE_COUNT];
uint8_t text_fg_clut[TEXT_MODE_COUNT];
uint8_t text_bg_clut[TEXT_MODE_COUNT];


/*
 * file scope
 */
static int syncDmaChan = 0;
static int rgbDmaChan = 0;
static VgaInitParams vgaParams;
static ScreenModeParams screenModeParams;


/*
 * build the sync data buffers
 */
static void buildSyncData(void) {
    const uint32_t sysClockKHz = clock_get_hz(clk_sys) / 1000;
    const uint32_t pioClockKHz = sysClockKHz;

    // compute number of pio ticks for a single pixel
    const float pioClocksPerPixel = (float) pioClockKHz / screenModeParams.pixel_clock_in_khz;

    // compute pio ticks for each phase of the scanline
    const uint32_t activeTicks =
            round(pioClocksPerPixel * (float) screenModeParams.active_x_pixels) - vga_sync_SETUP_OVERHEAD;
    const uint32_t fPorchTicks =
            round(pioClocksPerPixel * (float) screenModeParams.front_porch_x_pixels) - vga_sync_SETUP_OVERHEAD;
    const uint32_t syncTicks =
            round(pioClocksPerPixel * (float) screenModeParams.hsync_x_pixels) - vga_sync_SETUP_OVERHEAD;
    const uint32_t bPorchTicks =
            round(pioClocksPerPixel * (float) screenModeParams.back_porch_x_pixels) - vga_sync_SETUP_OVERHEAD;

    // compute sync bits
    const uint32_t hSyncOff = (screenModeParams.hsync_pulse_positive ? 0b00 : 0b01) << vga_sync_WORD_SYNC_OFFSET;
    const uint32_t hSyncOn = (screenModeParams.hsync_pulse_positive ? 0b01 : 0b00) << vga_sync_WORD_SYNC_OFFSET;
    const uint32_t vSyncOff = (screenModeParams.vsync_pulse_positive ? 0b00 : 0b10) << vga_sync_WORD_SYNC_OFFSET;
    const uint32_t vSyncOn = (screenModeParams.vsync_pulse_positive ? 0b10 : 0b00) << vga_sync_WORD_SYNC_OFFSET;

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
    channel_config_set_dreq(&syncDmaChanConfig,
                            pio_get_dreq(VGA_PIO, SYNC_SM, true)); // transfer when there's space in fifo

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
    float pioClocksPerScaledPixel = pioClockKHz * screenModeParams.pixel_scale_x / screenModeParams.pixel_clock_in_khz;
    while (pioClocksPerScaledPixel > 31) {
        ++pioClkDiv;
        pioClockKHz = sysClockKHz / pioClkDiv;
        pioClocksPerScaledPixel = pioClockKHz * screenModeParams.pixel_scale_x / screenModeParams.pixel_clock_in_khz;
    }

    // compute number of pio ticks for a single pixel
    const float pioClocksPerPixel = pioClockKHz / screenModeParams.pixel_clock_in_khz;
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

    // initalize sync pins for pio
    for (uint i = 0; i < RGB_PINS_COUNT; ++i) {
        pio_gpio_init(VGA_PIO, RGB_PINS_START + i);
    }

    // add rgb pio program
    pio_sm_set_consecutive_pindirs(VGA_PIO, RGB_SM, RGB_PINS_START, RGB_PINS_COUNT, true);
    pio_set_y(VGA_PIO, RGB_SM, screenModeParams.vga_virtual_pixel_width - 1);

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
    channel_config_set_write_increment(&rgbDmaChanConfig, false);           // don;t increment write
    channel_config_set_dreq(&rgbDmaChanConfig, pio_get_dreq(VGA_PIO, RGB_SM, true));

    // setup the dma channel and set it going
    dma_channel_configure(rgbDmaChan, &rgbDmaChanConfig, &VGA_PIO->txf[RGB_SM], rgbDataBufferEven,
                          screenModeParams.vga_virtual_pixel_width,
                          false);
    dma_channel_set_irq0_enabled(rgbDmaChan, true);
}

/*
 * dma interrupt handler
 */
static void __time_critical_func(dmaIrqHandler)(void) {
    static int currentTimingLine = -1;
    static int currentDisplayLine = -1;

    if (dma_hw->ints0 & (1u << syncDmaChan)) {
        dma_hw->ints0 = 1u << syncDmaChan;

        if (++currentTimingLine >= screenModeParams.total_y_pixels) {
            currentTimingLine = 0;
            currentDisplayLine = 0;
        }

        if (currentTimingLine < screenModeParams.vsync_y_pixels) {
            dma_channel_set_read_addr(syncDmaChan, syncDataSync, true);
        } else if (currentTimingLine < (screenModeParams.vsync_y_pixels + screenModeParams.back_porch_y_pixels)) {
            dma_channel_set_read_addr(syncDmaChan, syncDataPorch, true);
        } else if (currentTimingLine < (screenModeParams.total_y_pixels - screenModeParams.front_porch_y_pixels)) {
            dma_channel_set_read_addr(syncDmaChan, syncDataActive, true);
        } else {
            dma_channel_set_read_addr(syncDmaChan, syncDataPorch, true);
        }
    }


    if (dma_hw->ints0 & (1u << rgbDmaChan)) {
        dma_hw->ints0 = 1u << rgbDmaChan;

        divmod_result_t pxLineVal = divmod_u32u32(++currentDisplayLine, screenModeParams.pixel_scale_y);
        uint32_t pxLine = to_quotient_u32(pxLineVal);
        uint32_t pxLineRpt = to_remainder_u32(pxLineVal);

        dma_channel_set_read_addr(rgbDmaChan, (pxLine & 1) ? rgbDataBufferOdd : rgbDataBufferEven, true);

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
static void vgaLoop() {
    uint64_t frame_counter = 0;

    while (1) {
        uint32_t message = multicore_fifo_pop_blocking();
        if (message == END_OF_FRAME_MSG) {
            if (vgaParams.endOfFrameFn)
                vgaParams.endOfFrameFn(frame_counter);
            ++frame_counter;
        } else if ((message & END_OF_SCANLINE_MSG) != 0) {
            if (vgaParams.endOfScanlineFn)
                vgaParams.endOfScanlineFn();
        } else if (message & 0x01) {
            vgaParams.scanlineFn(message & 0xfff, rgbDataBufferOdd);
        } else {
            vgaParams.scanlineFn(message & 0xfff, rgbDataBufferEven);
        }
    }
}


/*
 * initialise the vga
 */
void vgaInit(VgaInitParams params, ScreenModeParams modeParams) {
    vgaParams = params;
    screenModeParams = modeParams;

    vgaInitSync();
    vgaInitRgb();

    initDma();

    pio_sm_set_enabled(VGA_PIO, SYNC_SM, true);
    pio_sm_set_enabled(VGA_PIO, RGB_SM, true);

    multicore_launch_core1(vgaLoop);
}


void screen_Mode0_Scanline(uint16_t raster_y, uint16_t pixels[screenModeParams.vga_virtual_pixel_width]) {
    // Draw chars
    if (raster_y >= 0 && raster_y < VGA_VIRTUAL_HEIGHT) {
        // Grab row index based off current raster Y position
        uint8_t rowIdx = raster_y / CHAR_HEIGHT;
        uint8_t lineIdx = raster_y % 8;

        for (uint16_t colIdx = 0; colIdx < (TEXT_MODE_WIDTH * CHAR_WIDTH); colIdx += CHAR_WIDTH) {
            uint8_t charIdx = colIdx / CHAR_WIDTH;
            uint16_t bufferIdx = rowIdx * TEXT_MODE_WIDTH + charIdx;
            uint8_t character = text_buffer[bufferIdx];
            unsigned char line = petscii[character][lineIdx];
            uint16_t fgcolor = palette_buffer[text_fg_clut[bufferIdx]];
            uint16_t bgcolor = palette_buffer[text_bg_clut[bufferIdx]];

            pixels[colIdx + 0] = CHECK_BIT(line, (7 - 0)) ? fgcolor : bgcolor;
            pixels[colIdx + 1] = CHECK_BIT(line, (7 - 1)) ? fgcolor : bgcolor;
            pixels[colIdx + 2] = CHECK_BIT(line, (7 - 2)) ? fgcolor : bgcolor;
            pixels[colIdx + 3] = CHECK_BIT(line, (7 - 3)) ? fgcolor : bgcolor;
            pixels[colIdx + 4] = CHECK_BIT(line, (7 - 4)) ? fgcolor : bgcolor;
            pixels[colIdx + 5] = CHECK_BIT(line, (7 - 5)) ? fgcolor : bgcolor;
            pixels[colIdx + 6] = CHECK_BIT(line, (7 - 6)) ? fgcolor : bgcolor;
            pixels[colIdx + 7] = CHECK_BIT(line, (7 - 7)) ? fgcolor : bgcolor;
        }
    }
}

void screen_Mode1_Scanline(uint16_t raster_y, uint16_t pixels[screenModeParams.vga_virtual_pixel_width]) {
    uint16_t screenWidth = screenModeParams.vga_virtual_pixel_width;
    uint16_t screenHeight = screenModeParams.vga_virtual_pixel_height;

    // Draw chars
    if (raster_y >= 0 && raster_y < screenModeParams.vga_virtual_pixel_height) {
        // Grab row index based off current raster Y position
        uint8_t rowIdx = raster_y / CHAR_HEIGHT;
        uint8_t lineIdx = raster_y % 8;

        for (uint16_t colIdx = 0; colIdx < (TEXT_MODE_WIDTH * CHAR_WIDTH); colIdx += CHAR_WIDTH) {
            uint8_t charIdx = colIdx / CHAR_WIDTH;
            uint16_t bufferIdx = rowIdx * TEXT_MODE_WIDTH + charIdx;
            uint8_t character = text_buffer[bufferIdx];
            unsigned char line = petscii[character][lineIdx];
            uint16_t fgcolor = palette_buffer[text_fg_clut[bufferIdx]];
            uint16_t bgcolor = palette_buffer[text_bg_clut[bufferIdx]];

            pixels[colIdx + 0] = CHECK_BIT(line, (7 - 0)) ? fgcolor : bgcolor;
            pixels[colIdx + 1] = CHECK_BIT(line, (7 - 1)) ? fgcolor : bgcolor;
            pixels[colIdx + 2] = CHECK_BIT(line, (7 - 2)) ? fgcolor : bgcolor;
            pixels[colIdx + 3] = CHECK_BIT(line, (7 - 3)) ? fgcolor : bgcolor;
            pixels[colIdx + 4] = CHECK_BIT(line, (7 - 4)) ? fgcolor : bgcolor;
            pixels[colIdx + 5] = CHECK_BIT(line, (7 - 5)) ? fgcolor : bgcolor;
            pixels[colIdx + 6] = CHECK_BIT(line, (7 - 6)) ? fgcolor : bgcolor;
            pixels[colIdx + 7] = CHECK_BIT(line, (7 - 7)) ? fgcolor : bgcolor;
        }
    }

    drawSprites(screenWidth, screenHeight, raster_y, pixels);

    // About 60 fps
    if (raster_y == screenModeParams.vga_virtual_pixel_height - 1) {
        updateSprites();
    }
}

void screen_Mode2_Scanline(uint16_t raster_y, uint16_t pixels[screenModeParams.vga_virtual_pixel_width]) {
//    memcpy(pixels, toucan + raster_y * VGA_VIRTUAL_WIDTH, VGA_VIRTUAL_WIDTH * sizeof(uint16_t));
}

void screen_Mode3_Scanline(uint16_t raster_y, uint16_t pixels[screenModeParams.vga_virtual_pixel_width]) {
    uint16_t screenWidth = screenModeParams.vga_virtual_pixel_width;
    uint16_t screenHeight = screenModeParams.vga_virtual_pixel_height;

    drawMap(screenWidth, screenHeight, raster_y, pixels);
    drawSprites(screenWidth, screenHeight, raster_y, pixels);

    // About 60 fps
    if (raster_y == screenModeParams.vga_virtual_pixel_height - 1) {
        updateSprites();
    }
}


void initCharMode() {
    initPalette();
    for (uint16_t i = 0; i < TEXT_MODE_COUNT; i++) {
        text_buffer[i] = 32;    // clear to space
        text_fg_clut[i] = 1;
    }
}


/**
 * Initialized the palette buffer.
 * There are #PALETTECOUNT entries.
 * Each containing a 12-bit color defined as: 0x0RGB for a total of 4096 colors.
 * Just like and OCS Amiga but with more palette entries.  :-)
 */
void initPalette() {
    for (int i = 0; i < PALETTECOUNT; i++) {
        palette_buffer[i] = 0x025b;
    }
    // define some basic values
    palette_buffer[1] = 0x06bf;
}


/**
 * Updates the screen cursor position based on character cells
 * @param x screen x column
 * @param y screen y row
 */
void setTextCursor(uint8_t x, uint8_t y) {
    cursor_x = x;
    cursor_y = y;
}


/**
 * Defines the color definition of a palette entry.
 * @param paletteNumber Palette entry to update
 * @param color Color definition in 0x0RGB format
 */
void setPalette(uint8_t paletteNumber, uint16_t color) {
    if (paletteNumber >= 0 && paletteNumber < PALETTECOUNT - 1) {
        palette_buffer[paletteNumber] = color;
    }
}


/**
 * Changes the text buffer foreground color at position X/Y to one of the palette entries.
 * @param x X position in characters (COLS)
 * @param y Y position in characters (ROWS)
 * @param paletteNumber Palette entry number
 */
void setFGColor(uint8_t x, uint8_t y, uint8_t paletteNumber) {
    text_fg_clut[y * TEXT_MODE_WIDTH + x] = paletteNumber;
}


/**
 * Changes the text buffer background color at position X/Y to one of the palette entries.
 * @param x X position in characters (COLS)
 * @param y Y position in characters (ROWS)
 * @param paletteNumber Palette entry number
 */

void setBGColor(uint8_t x, uint8_t y, uint8_t paletteNumber) {
    text_bg_clut[y * TEXT_MODE_WIDTH + x] = paletteNumber;
}


/**
 * Draws a character to the text buffer
 * @param x X position in characters (column)
 * @param y Y position in characters (rows)
 * @param character Character (from PETSCII font)
 */
void drawCharacter(uint8_t x, uint8_t y, uint8_t character) {
    uint16_t idx = y * TEXT_MODE_WIDTH + x;
    if (idx >= TEXT_MODE_COUNT) return;
    text_buffer[idx] = character;
}

/**
 * Draws an ASCII character to the text buffer (uses ascii_to_petscii buffer)
 * @param x X position in characters (column)
 * @param y Y position in characters (rows)
 * @param character Character (from ASCII to PETSCII buffer)
 */
void drawAsciiCharacter(uint8_t x, uint8_t y, uint8_t character) {
    uint16_t idx = y * TEXT_MODE_WIDTH + x;
    if (idx >= TEXT_MODE_COUNT) return;
    text_buffer[idx] = ascii_to_petscii[character];
}


/**
 * Draws a string of ASCII characters that are mapped to PETSCII characters.
 * NOTE, only the basic upper-case alpha characters, numbers and a few symbols are supported.
 * Also, drawing starts at the current cursor position and will scroll the screen up if it wraps
 * passed the last character position (lower right)
 *
 * @param str String to print out
 */
void drawCharacterString(char *str) {
    while (*str) {
        text_write(*str++);
    }
}

void text_write(unsigned char c) {
    if (c == '\n') {
        // newline pushes cursor down but not to the left
        cursor_y++;
        if (cursor_y >= TEXT_MODE_HEIGHT) {
            cursor_y = TEXT_MODE_HEIGHT - 1;
            shiftCharactersUp();
        }
    } else if (c == '\r') {
        // carriage return pushes cursor down and to the left 0 position
        cursor_x = 0;
        cursor_y++;
        if (cursor_y >= TEXT_MODE_HEIGHT) {
            cursor_y = TEXT_MODE_HEIGHT - 1;
            shiftCharactersUp();
        }
    } else if (c == '\t') {
        // tab moves the cursor over TAB_SPACES.  It will wrap around and adjust the screen.
        cursor_x = cursor_x + TAB_SPACES;
        if (cursor_x >= TEXT_MODE_WIDTH) {
            cursor_x = cursor_x % TEXT_MODE_WIDTH;
            cursor_y++;

            if (cursor_y >= TEXT_MODE_HEIGHT) {
                shiftCharactersUp();
                cursor_y = TEXT_MODE_HEIGHT - 1;
            }
        }
    } else {
        drawAsciiCharacter(cursor_x, cursor_y, c);
        cursor_x++;
        if (cursor_x >= TEXT_MODE_WIDTH) {
            cursor_x = 0;
            cursor_y++;

            if (cursor_y >= TEXT_MODE_HEIGHT) {
                shiftCharactersUp();
                cursor_y = TEXT_MODE_HEIGHT - 1;
            }
        }
        // TODO handle wrapping screen
        // hint: reset cursor_x to 0 and use screen shift up if cursor_x > max x/y cursor position (bottom right)
    }
}


/**
 * Shifts the entire text buffer screen up one line.
 * Destroys the very top line and draws a blank line at the bottom.
 *
 * TODO look into using memcopy for this!!!
 */
void shiftCharactersUp() {
    for (int y = 1; y < TEXT_MODE_HEIGHT; y++) {
        for (int x = 0; x < TEXT_MODE_WIDTH; x++) {
            text_buffer[((y - 1) * TEXT_MODE_WIDTH) + x] = text_buffer[(y * TEXT_MODE_WIDTH) + x];
            text_fg_clut[((y - 1) * TEXT_MODE_WIDTH) + x] = text_fg_clut[(y * TEXT_MODE_WIDTH) + x];
            text_bg_clut[((y - 1) * TEXT_MODE_WIDTH) + x] = text_bg_clut[(y * TEXT_MODE_WIDTH) + x];
        }
    }

    // now draw a blank line at the bottom with the current foreground/background color
    for (int i = 0; i < TEXT_MODE_WIDTH; i++) {
        setFGColor(i, TEXT_MODE_HEIGHT - 1, 1);
        setBGColor(i, TEXT_MODE_HEIGHT - 1, 0);
        drawCharacter(i, TEXT_MODE_HEIGHT - 1, BLANK_CHAR);
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
    x %= screenModeParams.vga_virtual_pixel_width;
    y %= VGA_VIRTUAL_HEIGHT;
    frame_buffer[y * screenModeParams.vga_virtual_pixel_width + x] = color;
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



