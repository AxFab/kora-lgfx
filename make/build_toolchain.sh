#!/bin/bash

set -e
SYSDIR=`pwd`
SRCDIR=$(readlink -f `dirname ${BASH_SOURCE[0]}`/..)
# --target
# --prefix
# --croos-gcc
OSPFX=i386-kora
echo "We need to parse options!"


function check_module {
    if [ ! -f $1/makefile ]; then
        echo "Update git submodule $1"
        git -C $SRCDIR/ submodule update $1
    fi
}

function standard_build {

    PACKAGE=$1

    echo "------------------------------"
    echo "Install $PACKAGE"
    cd $SYSDIR/var/
    if [ ! -f $PACKAGE.tar.xz ]
    then
        echo "Download $PACKAGE on `pwd`"
        # cp -v ./$PACKAGE.tar.xz var/$PACKAGE.tar.xz
        wget $3
    fi
    rm -rf $PACKAGE tmp/$PACKAGE
    echo "Extract $PACKAGE"
    tar xf $PACKAGE.tar.xz
    cd $PACKAGE
    echo "Patch $PACKAGE"
    # git init
    # git add .
    # git commit -m Initial
    if [ -f $SRCDIR/_scripts/$PACKAGE.patch ]
    then
        patch -u -p 1 -i $SRCDIR/_scripts/$PACKAGE.patch
    fi

    echo "Building $PACKAGE"
    mkdir -p $SYSDIR/var/tmp/$PACKAGE
    cd $SYSDIR/var/tmp/$PACKAGE
    $2 $1
    cd $SYSDIR
}

function standard_make {
    PACKAGE=$1
    $SYSDIR/var/$PACKAGE/configure --prefix=$SYSDIR
    make
    make install
}

function binutils_make {
    pushd $SYSDIR/var/$PACKAGE/ld
    automake
    autoconf
    popd
    PACKAGE=$1
    $SYSDIR/var/$PACKAGE/configure --target=$OSPFX --prefix=$SYSDIR --with-sysroot=$SYSDIR --disable-nls --disable-werror --enable-shared
    make
    make install
}

function gcc_make {
    PACKAGE=$1
    pushd $SYSDIR/var/$PACKAGE/libstdc++-v3
    automake
    autoconf
    popd
    pushd $SYSDIR/var/$PACKAGE/gcc
    autoconf
    popd
    pushd $SYSDIR/var/$PACKAGE
    ./contrib/download_prerequisites
    autoconf
    popd
    $SYSDIR/var/$PACKAGE/configure --target=$OSPFX --prefix=$SYSDIR --with-sysroot=$SYSDIR --disable-nls --enable-shared -enable-languages=c,c++
    make all-gcc
    make all-target-libgcc
    make install-gcc
    make install-target-libgcc
}

# ------------------------------

echo "Setup system directory: $SYSDIR"
echo "Look for source at : $SRCDIR"
export PATH=$SYSDIR/bin:$PATH
mkdir -p {,usr/}{bin,lib} var

# ------------------------------

# Build OS-Specific toolchain
#
# We use generic script to build the packet
# Usage: standard_build <PACKAGE> <make_commands> [download-url]
# With standard_make: configure && make && make install


AUTOCONF='autoconf-2.64'
AUTOMAKE='automake-1.11.6'
MFOUR='m4-1.4.13'
NASM='nasm-2.14.02'
BUTILS='binutils-2.30'
GCC='gcc-7.3.0'


AUTOCONF_URL="ftp://ftp.gnu.org/pub/gnu/autoconf/$AUTOCONF.tar.xz"
AUTOMAKE_URL="http://ftp.gnu.org/gnu/automake/$AUTOMAKE.tar.xz"
MFOUR_URL="http://ftp.gnu.org/gnu/m4/$MFOUR.tar.xz"
NASM_URL="https://www.nasm.us/pub/nasm/releasebuilds/2.14.02/$NASM.tar.xz"
BUTILS_URL="https://ftp.gnu.org/gnu/binutils/$BUTILS.tar.xz"
GCC_URL="https://ftp.gnu.org/gnu/gcc/$GCC/$GCC.tar.xz"


if [ ! -f bin/m4 ]; then
    standard_build $MFOUR standard_make $MFOUR_URL
fi
if [ ! -f bin/automake ]; then
    standard_build $AUTOMAKE standard_make $AUTOMAKE_URL
fi
if [ ! -f bin/autoconf ]; then
    standard_build $AUTOCONF standard_make $AUTOCONF_URL
fi
if [ ! -f bin/nasm ]; then # TODO -- Build only for x86 and x86_64
    standard_build $NASM standard_make $NASM_URL
fi
if [ ! -f bin/$OSPFX-ld ]; then
    standard_build $BUTILS binutils_make $BUTILS_URL
fi

if [ ! -d usr/include ]; then
    # check_module kernel
    # check_module libs/libc
    echo "Re-install headers - Kora libc & kernel"
    mkdir -p usr/
    rm -rf usr/include
    cp -r "$SRCDIR/libs/libc/include" usr/
    cp -r "$SRCDIR/kernel/include/kernel" usr/include
fi

if [ ! -f bin/$OSPFX-gcc ]; then
    standard_build $GCC gcc_make $GCC_URL
fi

# We do have the tooling !
echo "OS-Specific toolchain is ready"
