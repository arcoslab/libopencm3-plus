##
## This file is part of the libopencm3 project.
##
## Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
##
## This library is free software: you can redistribute it and/or modify
## it under the terms of the GNU Lesser General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public License
## along with this library.  If not, see <http://www.gnu.org/licenses/>.
##

PLATFORM	= stm32f4
LIBOPENCM3	= libopencm3_$(PLATFORM).a
LIBNAME		= libopencm3_plus_stm32f4discovery

PREFIX	        ?= arm-none-eabi
# PREFIX	?= arm-elf
CC		= $(PREFIX)-gcc
AR		= $(PREFIX)-ar

LIBOPENCM3_DIR ?= ../../../../libopencm3
ifeq ($(wildcard ../../../../libopencm3/lib/$(LIBOPENCM3)),)
ifneq ($(strip $(shell which $(CC))),)
LIBOPENCM3_DIR := $(shell dirname `which $(CC)`)/../$(PREFIX)
endif
else
ifeq ($(V),1)
$(info We seem to be building the example in the source directory. Using local library!)
endif
endif

$(info tool $(LIBOPENCM3_DIR))

CFLAGS		= -Os -g \
		  -Wall -Wextra -Wimplicit-function-declaration \
		  -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes \
		  -Wundef -Wshadow \
		  -I$(LIBOPENCM3_DIR)/include\
		  -I../../../include -fno-common \
		  -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 \
		  -Wstrict-prototypes \
		  -ffunction-sections -fdata-sections -MD -DSTM32F4 -DSTM32F4DISCOVERY
LDFLAGS		= 
# ARFLAGS	= rcsv
ARFLAGS		= rcs

OBJS		:= cdcacm_stm32f4discovery.o

VPATH += ../:../cdcacm_one_serial:../newlib:../utils

include ../../Makefile.include
