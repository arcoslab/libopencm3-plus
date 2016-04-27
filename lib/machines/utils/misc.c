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

#include <libopencm3-plus/utils/misc.h>
#include <libopencm3/stm32/gpio.h>

void wait(int a) {
  int i, j;
  for (j=0; j< a; j++) {
    for (i = 0; i < 100000; i++)   /* Wait a bit. */
      __asm__("nop");
  }
}

void printled(int rep, int gpiop, int pin) {
  int i, j;
  for (j=0; j< 10; j++) {
    for (i = 0; i < 1000000; i++)   /* Wait a bit. */
      __asm__("nop");
  }
  for (i=0; i<rep; i++) {
    gpio_set(gpiop, pin);
    for (j = 0; j < 8000000; j++)   /* Wait a bit. */
      __asm__("nop");
    gpio_clear(gpiop, pin);
    for (j = 0; j < 8000000; j++)   /* Wait a bit. */
      __asm__("nop");
  }
}

void printled2(int rep, int time, int gpiop, int pin) {
  int i, j;
  for (j=0; j< 100; j++) {
    for (i = 0; i < time*1000; i++)   /* Wait a bit. */
      __asm__("nop");
  }
  for (i=0; i<rep; i++) {
    gpio_set(gpiop, pin);
    for (j = 0; j < time*100000; j++)   /* Wait a bit. */
      __asm__("nop");
    gpio_clear(gpiop, pin);
    for (j = 0; j < time*100000; j++)   /* Wait a bit. */
      __asm__("nop");
  }
}

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

void my_itoa(int n, char s[])
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
