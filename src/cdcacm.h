/*
 * Copyright (C) 2013 ARCOS-Lab Universidad de Costa Rica
 * Author: Federico Ruiz Ugalde <memeruiz@gmail.com>
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
#ifndef CDCACM_H
#define CDCACM_H
#include <libopencm3/usb/usbd.h>

#define CDCACM_PACKET_SIZE 	64
#define CDCACM_READ_BUF_SIZE CDCACM_PACKET_SIZE*4
#define IRQ_PRI_USB		(2 << 4)

void cdcacm_poll(void);

void cdcacm_init(void);

int cdcacm_open(const char *path, int flags, int mode);

int cdcacm_close(int fd);

long cdcacm_write(int fd, const char *ptr, int len);

long cdcacm_read(int fd, char *ptr, int len);

//static const struct usb_device_descriptor dev;

//extern usbd_device * usbdev;

//extern cbuf_t cdc_cbuf_in;
#endif
