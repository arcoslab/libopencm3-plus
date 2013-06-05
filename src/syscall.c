#include <errno.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "cdcacm.h"
#include <stdio.h>
#include "mutex.h"
#include <libopencm3/stm32/f4/gpio.h>

#undef errno
extern int errno;
#define OS_MAX_TASKS 10
#define HEAPSIZE 32768
typedef struct {
  const char *name;
  int  (*open)( const char *path, int flags, int mode );
  int  (*close)( int fd );
  long (*write)( int fd, const char *ptr, size_t len );
  long (*read)( int fd, char *ptr, size_t len );
} devoptab_t;

int cdcacm_open(const char *path, int flags, int mode) {
  return(0);
}

int cdcacm_close(int fd) {
  return(0);
}

long cdcacm_write(int fd, const char *ptr, int len) {
  int index;
  static char buf[CDCACM_PACKET_SIZE];
  static int buf_pos=0;
  /* For example, output string by UART */
  for(index=0; index<len; index++)
    {
      buf[buf_pos]=ptr[index];
      buf_pos+=1;
      if (ptr[index] == '\n')
	{
	  buf[buf_pos]='\r';
	  buf_pos+=1;
	  while (usbd_ep_write_packet(usbdev, 0x82, buf, buf_pos) ==0);
	  buf_pos=0;
	}
    }
  return len;
}

long cdcacm_read(int fd, char *ptr, int len) {
  //printf("read len %d\n", len);
  while (cbuf_used(&cdc_cbuf_in) < len) {
  };
  return(cbuf_pop(&cdc_cbuf_in, ptr, len));
}

const devoptab_t dotab_cdcacm = { "cdcacm",
                                   cdcacm_open,
                                   cdcacm_close,
                                   cdcacm_write,
                                   cdcacm_read };

const devoptab_t *devoptab_list[] = {
   &dotab_cdcacm,  /* standard input */
   &dotab_cdcacm,  /* standard output */
   &dotab_cdcacm,  /* standard error */
   0             /* terminates the list */
};

long _write(int fd, const void *buf, size_t cnt) {
  return (*devoptab_list[fd]).write(fd, buf, cnt);
}


long _read(int fd, char *buf, size_t cnt) {
  return (*devoptab_list[fd]).read(fd, buf, cnt);
}


int _open(const char *file, int flags, int mode) {
  int which_devoptab = 0;
  int fd = -1;
  /* search for "file" in dotab_list[].name */
  do {
    if( strcmp( (*devoptab_list[which_devoptab]).name, file ) == 0 ) {
      fd = which_devoptab;
      break;
    }
  } while( devoptab_list[which_devoptab++] );
  /* if we found the requested file/device,
     then invoke the device's open_r() method */
  if( fd != -1 ) (*devoptab_list[fd]).open(file, flags, mode );
  /* it doesn't exist! */
  else errno = ENODEV;
  return fd;
}

long _close(int fd) {
  return (*devoptab_list[fd]).close(fd);
}

unsigned char _heap[HEAPSIZE];


#include <string.h>
 /* reverse:  reverse string s in place */
int strlen2(char s[]) {
  int i=0;
  while (s[i] != '\0') {
    i++;
  }
  return(i+1);
}


void reverse(char s[])
{
  int i, j;
  char c;
  for (i = 0, j = strlen2(s)-2; i<j; i++, j--) {
    c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
}

void itoa(int n, char s[])
{
  int i, sign;
  if ((sign = n) < 0)  /* record sign */
    n = -n;          /* make n positive */
  i = 0;
  do {       /* generate digits in reverse order */
    s[i++] = n % 10 + '0';   /* get next digit */
  } while ((n /= 10) > 0);     /* delete it */
  if (sign < 0)
    s[i++] = '-';
  s[i] = '\0';
  reverse(s);
}


caddr_t _sbrk(int incr) {
  static unsigned char *heap_end;
  unsigned char *prev_heap_end;
  /* debugging
  write( 2, "incr: ", 4);
  char incr_c[15];
  itoa(incr, incr_c);
  write( 2, incr_c, strlen2(incr_c));
  write( 2, "\n", 1);
  */
/* initialize */
  if( heap_end == 0 ) heap_end = _heap;
  prev_heap_end = heap_end;
  if( heap_end + incr -_heap > HEAPSIZE ) {
/* heap overflow—announce on stderr */
    write( 2, "Heap overflow!\n", 15 );
    abort();
  }
  heap_end += incr;
  return (caddr_t) prev_heap_end;
}

int _stat(const char *file, struct stat *pstat) {
  pstat->st_mode = S_IFCHR;
  return 0;
}

int _fstat(int fd, struct stat *pstat) {
  pstat->st_mode = S_IFCHR;
  return 0;
}

off_t _lseek(int fd, off_t pos, int whence) {
   return 0;
}
