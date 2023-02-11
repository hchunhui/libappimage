#!/bin/sh
git clone https://github.com/libfuse/libfuse.git &&
cd libfuse &&
git checkout d04687923194d906fe5ad82dcd546c9807bf15b6 &&
sed -i '/AM_ICONV/d' configure.ac &&
./makeconf.sh &&
mkdir build &&
cd build &&
CFLAGS="-Os" ../configure &&
make &&
make DESTDIR="$PWD/out" install &&
cp out/usr/local/lib/libfuse.so.2 ../.. &&
cd ../.. &&
rm -rf libfuse &&
strip libfuse.so.2 &&
python2 -c 'import sys, zlib; sys.stdout.write(zlib.compress(sys.stdin.read(), 9))' < libfuse.so.2 > libfuse.so.2.zlib &&
xxd -i libfuse.so.2.zlib > libfuse2inc.h &&
rm -f libfuse.so.2 libfuse.so.2.zlib
