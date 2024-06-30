# PicoVic

***

PicoVic is an implementation of a crude (but fun) "video chip" using a Raspberry Pico.

The ultimate goal of this project (hopefully), is to have an easy way to add nice VGA graphics to their homebrew
computers.

There are lots of people (including myself) that have designed SBC's (Single Board Computers) that struggle to find a
modern video solution. A popular approach is to use a **TMS9918** which is great. However, that solution is NTSC or PAL,
requires complex circuitry to handle the lack of DRAM and works with a CRT. Not to mentioned they are not created any
more.

If you want a drop-in replacement for the TMS9918, I recommend you follow the **Pico9918** project here:

https://github.com/visrealm/pico9918

This project is not a replacement for the TMS9918. But it aims to be similar with added benefits such as:

1) 4096 colors
2) 40x30 text mode (using PETSCII fonts at the moment)

TODO - finish this README with all of the awesome features already working.  :-)


