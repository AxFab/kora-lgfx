#!/bin/bash
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

SRCDIR=``
GENDIR=``
PAKDIR=``

# Create a build directory
mkdir -p "${GENDIR}"

# Configure
cd ${GENDIR}
${SRCDIR}/configure --prefix=${PAKDIR}

# Install (with .pc / )
cd ${GENDIR}
make install

# Create package .deb
SIZE=`du "${PAKDIR}" -h --max-depth=0 | cut -f 1`

mkdir -p ${GENDIR}/debian
cat > ${GENDIR}/debian/control << EOF
Package: libgfx
Version: 1.0
Section: custom
Priority: optional
Architecture: all
Essential: no
Installed-Size: ${SIZE}
Maintainer: koraos.net
Description: Library for basic graphic utilities
EOF

mkdir -p ${GENDIR}/debian/usr/{bin,lib,include}
cp liblgfx.so debian/usr/lib/libgfx.so.1.0.0
ln -s libgfx.so  debian/usr/lib/libgfx.so.1.0
ln -s libgfx.so.1.0  debian/usr/lib/libgfx.so.1.0.0

cp gfx.h debian/usr/include/gfx.h

cd ${GENDIR}
dpkg-deb --build debian
mv debian.deb libgfx-1.0_amd64.deb
