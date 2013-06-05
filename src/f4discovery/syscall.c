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
#include "utils.h"
#include "cdcacm.h"

#undef errno
extern int errno;
#define HEAPSIZE 32768

typedef struct {
  const char *name;
  int  (*open)( const char *path, int flags, int mode );
  int  (*close)( int fd );
  long (*write)( int fd, const char *ptr, size_t len );
  long (*read)( int fd, char *ptr, size_t len );
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

caddr_t _sbrk(int incr) {
  static unsigned char *heap_end;
  unsigned char *prev_heap_end;
  ///* debugging
  write( 2, "Asking for: ", 12);
  char incr_c[15];
  itoa(incr, incr_c);
  write( 2, incr_c, strlen2(incr_c));
  write( 2, " bytes.\n", 8);
  //*/
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
