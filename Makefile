#      This file is part of the KoraOS project.
#  Copyright (C) 2015-2021  <Fabien Bavent>
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

install: $(prefix)/lib/libgfx.so install-headers

include $(topdir)/make/build.mk
include $(topdir)/make/check.mk
include $(topdir)/make/targets.mk

CFLAGS ?= -Wall -Wextra -Wno-unused-parameter -ggdb

# -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

disto ?= kora
use_png ?= $(shell $(PKC) --exists libpng && echo 'y')


# -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
SRCS_l += $(wildcard $(srcdir)/*.c)
SRCS_l += $(srcdir)/addons/disto-$(disto).c


CFLAGS_l += $(CFLAGS)
CFLAGS_l += -I$(topdir)/include
CFLAGS_l += -fPIC

ifneq ($(sysdir),)
CFLAGS_l += -I$(sysdir)/include
LFLAGS_l += -L$(sysdir)/lib
endif

LFLAGS_l += -lm


ifeq ($(use_png),y)
SRCS_l += $(srcdir)/addons/img-png.c
CFLAGS_l += -D__USE_PNG
LFLAGS_l += -lpng
endif

ifeq ($(use_ft),y)
SRCS_l += $(srcdir)/addons/ft-freetype.c
CFLAGS_l += -D__USE_FREETYPE
LFLAGS_l += -lfreetype2
endif

ifeq ($(disto),x11)
LFLAGS_l += -lX11
endif


$(eval $(call comp_source,l,CFLAGS_l))
$(eval $(call link_shared,gfx,SRCS_l,LFLAGS_l,l))


# -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

install-headers: $(patsubst $(topdir)/%,$(prefix)/%,$(wildcard $(topdir)/include/*.h))

$(prefix)/include/%.h: $(topdir)/include/%.h
	$(S) mkdir -p $(dir $@)
	$(V) cp -vrP $< $@


check: $(patsubst %,val_%,$(CHECKS))

ifeq ($(NODEPS),)
-include $(call fn_deps,SRCS_l,l)
endif
