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

#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <libopencm3-plus/utils/misc.h>
#include <libopencm3-plus/stm32f4discovery/leds.h>
#include <libopencm3-plus/cdcacm_one_serial/cdcacm_common.h>
#include <libopencm3-plus/utils/common.h>
#include <libopencm3-plus/newlib/syscall.h>

#undef errno
extern int errno;
#define HEAPSIZE 2048

typedef struct {
  const char *name;
  int  (*open)( const char *path, int flags, int mode );
  int  (*close)( int fd );
  long (*write)( int fd, const char *ptr, int len );
  long (*read)( int fd, char *ptr, int len );
} devoptab_t;

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


unsigned char _heap[HEAPSIZE];


long _write(int fd, const void *buf, size_t cnt);
long _read(int fd, char *buf, size_t cnt);
int _open(const char *file, int flags, int mode);
long _close(int fd);
caddr_t _sbrk(int incr);
int _stat(const char *file, struct stat *pstat);
int _fstat(int fd, struct stat *pstat);
off_t _lseek(int fd, off_t pos, int whence);



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




caddr_t _sbrk(int incr) {
  static unsigned char *heap_end;
  unsigned char *prev_heap_end;
  ///* debugging
  ///*
  static int first=0;
  if (first==2) {
    //write( 2, "Asking for: ", 12);
    char incr_c[15];
    my_itoa(incr, incr_c);
    //write( 2, incr_c, strlen2(incr_c));
    //write( 2, " bytes.\n", 8);
  } else {
    first++;
  }
  //*/
  //
/* initialize */
  if( heap_end == 0 ) heap_end = _heap;
  prev_heap_end = heap_end;
  if( heap_end + incr -_heap > HEAPSIZE ) {
/* heap overflow—announce on stderr */
    printled(10, LRED);
    printled(10, LORANGE);
    //write( 2, "Heap overflow!\n", 15 );
    abort();
  }
  heap_end += incr;
  return (caddr_t) prev_heap_end;
}


int _stat(NOT_USED const char *file, struct stat *pstat) {
  pstat->st_mode = S_IFCHR;
  return 0;
}


int _fstat(NOT_USED int fd, struct stat *pstat) {
  pstat->st_mode = S_IFCHR;
  return 0;
}


off_t _lseek(NOT_USED int fd, NOT_USED off_t pos, NOT_USED int whence) {
   return 0;
}

int poll(FILE *f) {
  return(cdcacm_in_poll(fileno(f)));
}
