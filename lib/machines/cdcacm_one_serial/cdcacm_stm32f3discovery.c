/*
 * Copyright (C) 2011  Black Sphere Technologies Ltd.
 * Written by Gareth McMullin <gareth@blacksphere.co.nz>
 * Modified by Federico Ruiz Ugalde <memeruiz@gmail.com> ARCOS-Lab
 * Universidad de Costa Rica.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* This file implements a the USB Communications Device Class - Abstract
 * Control Model (CDC-ACM) as defined in CDC PSTN subclass 1.2.
 *
 * The device's unique id is used as the USB serial number string.
 */

#include <libopencm3/stm32/f3/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>
#include <libopencm3/cm3/nvic.h>
#include <stdlib.h>
#include <libopencm3-plus/utils/misc.h>
//#include <libopencm3-plus/stm32f3discovery/leds.h>
#include <libopencm3-plus/utils/data_structs.h>
#include <libopencm3-plus/cdcacm_one_serial/cdcacm.h>
#include <libopencm3-plus/utils/common.h>
#include <libopencm3-plus/cdcacm_one_serial/cdcacm_common.h>

void cdcacm_init(void) {
  //system setup
  /* Enable clocks for GPIO port A (for GPIO_USART2_TX) and USART2. */
  rcc_usb_prescale_1();
  rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_USBEN);
  rcc_peripheral_enable_clock(&RCC_AHBENR, RCC_AHBENR_IOPAEN);

  /* Setup GPIO pin GPIO_USART2_TX/GPIO9 on GPIO port A for transmit. */
  gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO11 | GPIO12);
  gpio_set_af(GPIOA, GPIO_AF14, GPIO11| GPIO12);

  cdcacm_usb_init();
  nvic_set_priority(NVIC_USB_LP_CAN1_RX0_IRQ, IRQ_PRI_USB);
  nvic_enable_irq(NVIC_USB_LP_CAN1_RX0_IRQ);
  while (cdcacm_get_config() != 1) { wait(1); }; //wait until usb is configured
}

void usb_lp_can1_rx0_isr(void)
{
  usbd_poll(usbdev);
}
