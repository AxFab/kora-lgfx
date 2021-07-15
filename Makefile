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

include $(topdir)/make/global.mk
srcdir = $(topdir)/src

all: libgfx
# PACKAGE=libgfx


include $(topdir)/make/build.mk
include $(topdir)/make/check.mk
include $(topdir)/make/targets.mk

# -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

disto ?= kora
use_png ?= $(shell $(PKC) --exists libpng && echo 'y')


# -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
SRCS += $(wildcard $(srcdir)/*.c)
SRCS += $(srcdir)/addons/disto-$(disto).c

CFLAGS ?= -Wall -Wextra -Wno-unused-parameter -ggdb
CFLAGS += -I$(topdir)/include
CFLAGS += -fPIC

LFLAGS += -lm


ifeq ($(use_png),y)
SRCS += $(srcdir)/addons/img-png.c
CFLAGS += -D__USE_PNG $(shell $(PKC) --cflags libpng)
LFLAGS += $(shell $(PKC) --libs libpng)
endif


ifeq ($(disto),x11)
LFLAGS += -lX11
else ifeq ($(disto),kora)
CFLAGS += -Dmain=_main -D_GNU_SOURCE
endif


$(eval $(call link_shared,gfx,SRCS,LFLAGS))


# install-headers:
# 	$(S) mkdir -p $(prefix)/include
# 	$(S) cp -RpP -f $(topdir)/gfx.h $(prefix)/include/gfx.h
# 	$(S) cp -RpP -f $(topdir)/keycodes.h $(prefix)/include/keycodes.h


e_dist:
	@echo $(target_arch)-$(target_os)
e_srcs:
	@echo $(SRCS)

check: $(patsubst %,val_%,$(CHECKS))

ifeq ($(NODEPS),)
include $(call fn_deps,SRCS)
endif
