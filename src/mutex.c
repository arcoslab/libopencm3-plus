#include <libopencm3/stm32/f4/gpio.h>
#include "mutex.h"


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


void printled(int a, int gpiop, int pin) {
  int i, j;
  for (j=0; j< 10; j++) {
    for (i = 0; i < 1000000; i++)   /* Wait a bit. */
      __asm__("nop");
  }
  for (i=0; i<a; i++) {
    gpio_set(gpiop, pin);
    for (j = 0; j < 8000000; j++)   /* Wait a bit. */
      __asm__("nop");
    gpio_clear(gpiop, pin);
    for (j = 0; j < 8000000; j++)   /* Wait a bit. */
      __asm__("nop");
  }
}
