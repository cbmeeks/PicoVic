# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/cbmeeks/sdk/pico/pico-sdk/tools/elf2uf2"
  "/data/gdrive/Projects/hardware/Pico/PicoVic/cmake-build-debug/elf2uf2"
  "/data/gdrive/Projects/hardware/Pico/PicoVic/cmake-build-debug/elf2uf2"
  "/data/gdrive/Projects/hardware/Pico/PicoVic/cmake-build-debug/elf2uf2/tmp"
  "/data/gdrive/Projects/hardware/Pico/PicoVic/cmake-build-debug/elf2uf2/src/ELF2UF2Build-stamp"
  "/data/gdrive/Projects/hardware/Pico/PicoVic/cmake-build-debug/elf2uf2/src"
  "/data/gdrive/Projects/hardware/Pico/PicoVic/cmake-build-debug/elf2uf2/src/ELF2UF2Build-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/data/gdrive/Projects/hardware/Pico/PicoVic/cmake-build-debug/elf2uf2/src/ELF2UF2Build-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/data/gdrive/Projects/hardware/Pico/PicoVic/cmake-build-debug/elf2uf2/src/ELF2UF2Build-stamp${cfgdir}") # cfgdir has leading slash
endif()
