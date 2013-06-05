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

#include "mutex.h"
#include "data_structs.h"
#include "misc.h"

int cbuf_used(cbuf_t* cbuf) {
  if (cbuf->last_pos >= cbuf->first_pos) {
    return(cbuf->last_pos-cbuf->first_pos);
  } else {
    return(cbuf->last_pos+CDCACM_READ_BUF_SIZE-cbuf->first_pos);
  }
}

int cbuf_free(cbuf_t* cbuf) {
  return(CDCACM_READ_BUF_SIZE-cbuf_used(cbuf));
}

int cbuf_pop(cbuf_t* cbuf, char *out, int out_len){
  int i;
  gpio_toggle(LBLUE);
  LOCK(cbuf->rmut);
  for (i=0; i<out_len; i++) {
    if (cbuf->first_pos == cbuf->last_pos) { //empty
      break;
    }
    out[i]=cbuf->buf[cbuf->first_pos];
    cbuf->first_pos++;
    if (cbuf->first_pos == CDCACM_READ_BUF_SIZE) {
      cbuf->first_pos=0;
    }
  }
  UNLOCK(cbuf->rmut);
  return(i);
}

int cbuf_append(cbuf_t* cbuf, char *in,  int in_size) {
  int i=0;
  gpio_toggle(LORANGE);
  LOCK(cbuf->wmut);
  if (cbuf_free(cbuf) > in_size) {
    for (i=0; i<in_size; i++) {
      cbuf->buf[cbuf->last_pos]=in[i];
      cbuf->last_pos++;
      if (cbuf->last_pos == CDCACM_READ_BUF_SIZE) {
	cbuf->last_pos=0;
      }
    }
  }
  UNLOCK(cbuf->wmut);
  return(i);
}
