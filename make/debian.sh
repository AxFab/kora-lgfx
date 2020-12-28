
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

