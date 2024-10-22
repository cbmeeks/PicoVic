###############################################################################
#   PicoVic
#
#   Copyright (c) 2024 Cecil Meeks
#   https://github.com/cbmeeks/PicoVic
###############################################################################
cmake_minimum_required(VERSION 3.26)

# Set environment variables
# I prefer setting this here instead of in .bashrc or other OS-specific file.  Basically, you just need to make sure
# to set your PICO_SDK_PATH environment variable to the location of the Pico SDK on your system.
set(ENV{PICO_SDK_PATH} "/home/cbmeeks/sdk/pico/pico-sdk")

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include($ENV{PICO_SDK_PATH}/tools/CMakeLists.txt)

# Set the name of the project (as PROJECT_NAME) and C/C++ standards
project(PicoVic C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

# Tell CMake where to find the executable source file
add_executable(${PROJECT_NAME}
        main.c
        vga.c
        pio_utils.c
        bit_helper.h
        screen_modes.c
        screen_modes.h
        chars.c
        tiles.c
        sprites.c
        res/sprites/sprite_defs.c
        res/tiles/tile_defs.c
        font_utils.c
)

# by default the header is generated into the build dir
# however, alternatively you can choose to generate it somewhere else (in this case in the source tree for check in)

# HSYNC and VSYNC are the same in all versions.  Only the RGB PIO needs to change for each resolution.
# TODO, look at making this more dynamic and changeable within the code
#pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/pio/hsync.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/pio)
#pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/pio/vsync.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/pio)
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/vga.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/pio)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

# must match with executable name and source file names
target_sources(${PROJECT_NAME} PRIVATE main.c vga.c pio_utils.c)

# Link to libraries (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME}
        pico_multicore
        pico_stdlib
        hardware_adc
        hardware_dma
        hardware_irq
        hardware_pio
)

# Enable USB output, disable UART output
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)


