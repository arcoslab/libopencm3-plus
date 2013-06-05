libopencm3-plus
===============

Additional tools for libopencm3 to be used together with the stm32f4discovery board.

This repository provides with easy-of-use cdcacm usb virtual serial port communication with a PC. It implements the basic syscalls from newlib to be able to use for example printf and scanf through the usb virtual serial port.

Included is also a dynamic circular buffer, some LED functions and macros specific to the stm32f4discovery board (LRED, LBLUE, LORANGE, LGREEN, printled)

Newlib _sbrk is also implemented, thus one can use malloc and other nice functions.

Remember to use the ld script included in this repo and not the one in libopencm3. This repo is also tested against libopencm3 only when it is used in your programs as a git submodule at the base of your project.
