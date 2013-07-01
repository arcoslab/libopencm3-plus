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

PREFIX		?= arm-none-eabi
#PREFIX		?= arm-elf

ifeq ($(DETECT_TOOLCHAIN),)
DESTDIR		?= /usr/local
else
DESTDIR		?= $(shell dirname $(shell readlink -f $(shell which $(PREFIX)-gcc)))/..
endif

INCDIR		:= $(DESTDIR)/$(PREFIX)/include
LIBDIR		:= $(DESTDIR)/$(PREFIX)/lib
SHAREDIR	:= $(DESTDIR)/$(PREFIX)/share/libopencm3/scripts
INSTALL		:= install

SRCLIBDIR:= $(realpath lib)
TARGETS:= machines/stm32f4discovery

# Be silent per default, but 'make V=1' will show all compiler calls.
ifneq ($(V),1)
Q := @
# Do not print "Entering directory ...".
MAKEFLAGS += --no-print-directory
endif

all: build

build: lib

LIB_DIRS:=$(wildcard $(addprefix lib/,$(TARGETS)))
$(LIB_DIRS):
	@printf "  BUILD   $@\n";
	$(Q)$(MAKE) --directory=$@ SRCLIBDIR=$(SRCLIBDIR)

lib: $(LIB_DIRS)
	$(Q)true

install: lib
	@printf "  INSTALL headers\n"
	$(Q)$(INSTALL) -d $(INCDIR)/libopencm3-plus
	$(Q)$(INSTALL) -d $(LIBDIR)
	$(Q)$(INSTALL) -d $(SHAREDIR)
	$(Q)cp -r include/libopencm3-plus/* $(INCDIR)/libopencm3-plus
	@printf "  INSTALL libs\n"
	$(Q)$(INSTALL) -m 0644 lib/*.a $(LIBDIR)
	@printf "  INSTALL ldscripts\n"
	$(Q)$(INSTALL) -m 0644 lib/*.ld $(LIBDIR)
	@printf "  INSTALL scripts\n"
	$(Q)$(INSTALL) -m 0644 scripts/* $(SHAREDIR)

# Bleh http://www.makelinux.net/make3/make3-CHP-6-SECT-1#make3-CHP-6-SECT-1
clean:
	$(Q)for i in $(LIB_DIRS) \
		     $(EXAMPLE_DIRS); do \
		if [ -d $$i ]; then \
			printf "  CLEAN   $$i\n"; \
			$(MAKE) -C $$i clean SRCLIBDIR=$(SRCLIBDIR) || exit $?; \
		fi; \
	done

.PHONY: build lib $(LIB_DIRS) install doc clean

