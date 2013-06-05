#ifndef MUTEX_H
#define MUTEX_H
#define LOCK(mutex) __asm__("CPSID i;"); if (mutex == 0) { mutex=1;
#define UNLOCK(mutex) mutex=0; }; __asm__("CPSIE i;");
#include "cdcacm.h"

#define LD3 GPIOD, GPIO13 //Orange
#define LD4 GPIOD, GPIO12 //green
#define LD5 GPIOD, GPIO14 //red
#define LD6 GPIOD, GPIO15 // blue
#define LORANGE LD3
#define LGREEN LD4
#define LRED LD5
#define LBLUE LD6

typedef struct {
  char buf[CDCACM_READ_BUF_SIZE];
  int first_pos;
  int last_pos;
  int wmut;
  int rmut;
} cbuf_t;

extern cbuf_t cdc_cbuf_in;
int cbuf_pop(cbuf_t* cbuf, char *out, int out_len);
int cbuf_append(cbuf_t* cbuf, char *in, int in_size);

#endif MUTEX_H

