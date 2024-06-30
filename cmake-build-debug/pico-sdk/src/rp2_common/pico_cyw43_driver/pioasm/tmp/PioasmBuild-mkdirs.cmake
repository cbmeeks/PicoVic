# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/cbmeeks/sdk/pico/pico-sdk/tools/pioasm"
  "/data/gdrive/Projects/hardware/Pico/PicoVic/cmake-build-debug/pioasm"
  "/data/gdrive/Projects/hardware/Pico/PicoVic/cmake-build-debug/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm"
  "/data/gdrive/Projects/hardware/Pico/PicoVic/cmake-build-debug/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/tmp"
  "/data/gdrive/Projects/hardware/Pico/PicoVic/cmake-build-debug/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/PioasmBuild-stamp"
  "/data/gdrive/Projects/hardware/Pico/PicoVic/cmake-build-debug/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src"
  "/data/gdrive/Projects/hardware/Pico/PicoVic/cmake-build-debug/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/PioasmBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/data/gdrive/Projects/hardware/Pico/PicoVic/cmake-build-debug/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/PioasmBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/data/gdrive/Projects/hardware/Pico/PicoVic/cmake-build-debug/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/PioasmBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()
