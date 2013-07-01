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

#ifndef CDCACM_COMMON_H
#define CDCACM_COMMON_H

#include <libopencm3/usb/usbd.h>


extern usbd_device * usbdev;

#define IRQ_PRI_USB		(2 << 4)

int cdcacm_in_poll(int fd);

void cdcacm_poll(void);

void cdcacm_usb_init(void);

int cdcacm_open(const char *path, int flags, int mode);

int cdcacm_close(int fd);

long cdcacm_write(int fd, const char *ptr, int len);

long cdcacm_read(int fd, char *ptr, int len);

int cdcacm_get_config(void);

void cdcacm_write_now(char* buf, int len);

#endif //CDCACM_COMMON
