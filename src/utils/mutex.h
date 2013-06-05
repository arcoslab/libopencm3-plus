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

#ifndef MUTEX_H
#define MUTEX_H

#define LOCK(mutex) __asm__("CPSID i;"); if (mutex == 0) { mutex=1;
#define UNLOCK(mutex) mutex=0; }; __asm__("CPSIE i;");

#endif MUTEX_H

