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

#ifndef UTILS_H
#define UTILS_H

#include <libopencm3/stm32/f4/gpio.h>

void wait(int a);

void printled(int a, int gpiop, int pin);

#define LD3 GPIOD, GPIO13 //Orange
#define LD4 GPIOD, GPIO12 //green
#define LD5 GPIOD, GPIO14 //red
#define LD6 GPIOD, GPIO15 // blue
#define LORANGE LD3
#define LGREEN LD4
#define LRED LD5
#define LBLUE LD6

int strlen2(char s[]);

/* reverse:  reverse string s in place */
void reverse(char s[]);

void itoa(int n, char s[]);

#endif // UTILS_H

