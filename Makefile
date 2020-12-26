#      This file is part of the SmokeOS project.
#  Copyright (C) 2015  <Fabien Bavent>
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Affero General Public License as
#  published by the Free Software Foundation, either version 3 of the
#  License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Affero General Public License for more details.
#
#  You should have received a copy of the GNU Affero General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
# -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
topdir ?= $(shell readlink -f $(dir $(word 1,$(MAKEFILE_LIST))))
gendir ?= $(shell pwd)

PACKAGE=lgfx
include $(topdir)/make/global.mk
include $(topdir)/make/build.mk


disto ?= kora
has_threads ?= $(shell $(topdir)/make/compiler.sh '__STDC_VERSION__ >= 201112 && !defined __STDC_NO_THREADS__' $(CC))
use_png ?= $(shell $(PKC) --exists libpng && echo 'y')



SRCS += $(wildcard $(srcdir)/*.c)
SRCS += $(srcdir)/addons/gfx-disto-$(disto).c

CFLAGS ?= -Wall -Wextra -ggdb
CFLAGS += -fPIC

# LFLAGS ?= -Wl,-z,defs
LFLAGS += -lm



ifeq ($(use_png),y)
SRCS += $(srcdir)/addons/gfx-png.c
CFLAGS += -D__USE_PNG $(shell $(PKC) --cflags libpng)
LFLAGS += $(shell $(PKC) --libs libpng)
endif

ifeq ($(has_threads),n)
SRCS += $(srcdir)/addons/threads_posix.c
CFLAGS += -I $(topdir)/addons $(shell $(PKC) --cflags pthread)
LFLAGS +=  $(shell $(PKC) --libs pthread)
endif

ifeq ($(disto),x11)
LFLAGS += -lX11
else ifeq ($(disto),kora)
CFLAGS += -Dmain=_main -D_GNU_SOURCE
endif


$(eval $(call link_shared,gfx,SRCS,LFLAGS))




include $(topdir)/make/check.mk
include $(topdir)/make/targets.mk

install-headers:
	$(S) mkdir -p $(prefix)/include
	$(S) cp -RpP -f $(topdir)/gfx.h $(prefix)/include/gfx.h
	$(S) cp -RpP -f $(topdir)/keycodes.h $(prefix)/include/keycodes.h


ifeq ($(NODEPS),)
include $(call fn_deps,SRCS)
endif
