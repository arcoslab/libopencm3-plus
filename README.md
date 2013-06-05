stm32f4_cdcacm_example
======================

CDCACM example code for the stm32f4discovery using libopencm3.

This example was made in order to have a nice initial working example for using the USB as a normal communication channel between the uC and a PC.

In this example the newlib syscalls have been implemented (at least the necessary ones), so that you can use scanf, printf and so on for sending and receiving data to a PC through the USB port.

This example is intended to be used on a stm32f4discovery board, but modify it to use it on other stm32f4 uC shouldn't be too difficult.

Some nice MACROS for accesing leds are included. One simply uses gpio_toggle(LRED) to turn the stm32f4discovery RED led on.

A circular buffer data structure is included.

printled(1, LRED), turns on and off one time the red led. (Time on and off is fixed, but you can always edit the code).

Take a look at utils and f4discovery directories to see what else you can use.
