/*
 * Copyright (C) 2013 ARCOS-Lab Universidad de Costa Rica
 * Written by Federico Ruiz Ugalde <memeruiz@gmail.com>
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

#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

#include "cdcacm/cdcacm.h"

typedef struct {
  char buf[CDCACM_READ_BUF_SIZE];
  int first_pos;
  int last_pos;
  int wmut;
  int rmut;
} cbuf_t;

extern cbuf_t cdc_cbuf_in;
int cbuf_used(cbuf_t* cbuf);

int cbuf_free(cbuf_t* cbuf);

int cbuf_pop(cbuf_t* cbuf, char *out, int out_len);

int cbuf_append(cbuf_t* cbuf, char *in,  int in_size);

#endif DATA_STRUCTS_H

