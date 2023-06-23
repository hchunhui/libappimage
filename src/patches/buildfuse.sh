#!/bin/sh
SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
git clone https://github.com/libfuse/libfuse.git &&
cd libfuse &&
git checkout d04687923194d906fe5ad82dcd546c9807bf15b6 &&
cp -v "$SCRIPTPATH/fuse_versionscript" lib/ &&
sed -i '/AM_ICONV/d' configure.ac &&
./makeconf.sh &&
mkdir build &&
cd build &&
CFLAGS="-Os -ffunction-sections -fdata-sections" LDFLAGS="-Wl,--gc-sections" ../configure --enable-util=no --enable-example=no &&
make &&
make DESTDIR="$PWD/out" install &&
cp out/usr/local/lib/libfuse.so.2 ../.. &&
cd ../.. &&
rm -rf libfuse &&
strip libfuse.so.2 &&
python2 -c 'import sys, zlib; sys.stdout.write(zlib.compress(sys.stdin.read(), 9))' < libfuse.so.2 > libfuse.so.2.zlib &&
xxd -i libfuse.so.2.zlib > libfuse2inc.h &&
rm -f libfuse.so.2 libfuse.so.2.zlib
