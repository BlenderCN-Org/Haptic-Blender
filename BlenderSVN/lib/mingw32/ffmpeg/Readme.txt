This directory contains ffmpeg library v0.10 from http://www.ffmpeg.org/releases/ (January 31th, 2012)
Compiled by Sergey Sharybin in msys environment

Quick notes
===========

1. Compile ffmpeg libraries and copy include files and libs here
2. put stdint.h and inttypes.h in subfolder msvc to prevent conflicts with mingw
3. add to libavutil\common.h after line 62
   #define inline __inline
4. modify stdint.h to make it compatible with MSVC C++ headers:
Add signed to int8_t, int16_t, int32_t, int64_t:
   typedef signed __int8     int8_t;
   typedef signed __int16    int16_t;
   typedef signed __int32    int32_t;
   typedef signed __int64    int64_t;
5. There were some problems with linking blender against installed dll-s which
   were solved by manual stripping dll (strip --strip-unneeded --strip-debug)
   and generating .lib-files for them

Environment configuration
=========================

1. Used 32 bit mingw/msys environment using this installer:
   http://sourceforge.net/projects/mingw/files/Automated%20MinGW%20Installer/mingw-get-inst
2. Used gcc-4.4.0 due to some incompatibilities between used libraries and newer compilers
   http://sourceforge.net/projects/mingw/files/MinGW/BaseSystem/GCC/Version4/Previous%20Release%20gcc-4.4.0/
3. Also used yasm: http://www.tortall.net/projects/yasm/wiki/Download
4. Used pthread library frmo mingw-w64 sourceforge files download page

Libraries configuration scripts
===============================

ffmpeg configuration
--------------------

LIBS="/home/sergey/lib"

./configure \
    --prefix=/home/sergey/lib/ffmpeg-0.10 \
    --enable-shared \
    --disable-static \
    --disable-avfilter \
    --disable-vdpau \
    --disable-bzlib \
    --disable-libgsm \
    --enable-libschroedinger \
    --disable-libspeex \
    --enable-libtheora \
    --enable-libvorbis \
    --enable-zlib \
    --enable-libvpx \
    --enable-stripping \
    --enable-runtime-cpudetect  \
    --disable-vaapi \
    --enable-libopenjpeg \
    --disable-nonfree \
    --enable-gpl \
    --disable-postproc \
    --disable-x11grab \
    --enable-libdirac \
    --enable-libmp3lame \
    --disable-librtmp \
    --enable-libx264 \
    --enable-libxvid \
    --disable-libopencore-amrnb \
    --disable-libopencore-amrwb \
    --disable-libdc1394 \
    --disable-version3 \
    --disable-debug \
    --enable-optimizations \
    --enable-ffplay \
    --disable-sse \
    --disable-ssse3 \
    --enable-w32threads \
    --disable-pthreads \
    --extra-cflags="-g -Wall -Wextra -I${LIBS}/lame/include -I${LIBS}/openjpeg/include -I${LIBS}/ogg/include -I${LIBS}/vorbis/include -I${LIBS}/theora/include -I${LIBS}/vpx/include -I${LIBS}/x264/include -I${LIBS}/xvid/include -I${LIBS}/dirac/include/dirac -I${LIBS}/schroedinger/include/schroedinger-1.0 -I${LIBS}/zlib/include" \
    --extra-ldflags="-Wl,--as-needed -static-libgcc -L${LIBS}/lame/lib -L${LIBS}/openjpeg/lib -L${LIBS}/ogg/lib -L${LIBS}/vorbis/lib -L${LIBS}/theora/lib -L${LIBS}/vpx/lib -L${LIBS}/x264/lib -L${LIBS}/xvid/lib -L${LIBS}/dirac/lib -L${LIBS}/schroedinger/lib -L${LIBS}/orc/lib -L${LIBS}/zlib/lib"

dirac configuration
-------------------

export CFLAGS="-g -Wall -Wextra"
export LDFLAGS="-Wl,--as-needed -static-libgcc"

./configure --prefix=/home/sergey/lib/dirac \
    --disable-shared \
    --enable-static

faad configuration
------------------

export CFLAGS="-g -Wall -Wextra"
export LDFLAGS="-Wl,--as-needed -static-libgcc"

./configure --prefix=/home/sergey/lib/faad \
    --disable-shared \
    --enable-static

lame configuration
------------------

export CFLAGS="-g -Wall -Wextra"
export LDFLAGS="-Wl,--as-needed -static-libgcc"

./configure --prefix=/home/sergey/lib/lame \
    --disable-shared \
    --enable-static \
    --enable-export=full \
    --with-fileio=sndfile \
    --without-vorbis \
    --enable-nasm \
    --with-pic \
    --disable-mp3x \
    --disable-mp3rtp \
    --disable-gtktest \
    --enable-export=full

libogg configuration
--------------------

export CFLAGS="-g -Wall -Wextra"
export LDFLAGS="-Wl,--as-needed -static-libgcc"

./configure \
    --prefix=/home/sergey/lib/ogg \
    --enable-static \
    --disable-shared

libvorbis configuration
-----------------------

export CFLAGS="-g -Wall -Wextra"
export LDFLAGS="-Wl,--as-needed -static-libgcc"

./configure \
    --prefix=/home/sergey/lib/vorbis \
    --enable-static \
    --disable-shared \
    --with-pic \
    --with-ogg=/home/sergey/lib/ogg/

libtheora configuration
-----------------------

export CFLAGS="-g -Wall -Wextra"
export LDFLAGS="-Wl,--as-needed -static-libgcc"

./configure \
    --prefix=/home/sergey/lib/theora \
    --enable-static \
    --disable-shared \
    --with-ogg=/home/sergey/lib/ogg \
    --with-vorbis=/home/sergey/lib/vorbis

libvpx configuration
--------------------

export CFLAGS="-g -Wall -Wextra"
export LDFLAGS="-Wl,--as-needed -static-libgcc"

./configure \
    --prefix=/home/sergey/lib/vpx \
    --disable-shared \
    --disable-install-bins \
    --disable-install-srcs \
    --extra-cflags="-g -Wall -Wextra"

openjpeg configuration
----------------------

export CFLAGS="-g -Wall -Wextra"
export LDFLAGS="-Wl,--as-needed -static-libgcc"

./configure \
    --prefix=/home/sergey/lib/openjpeg \
    --enable-static \
    --disable-shared

orc configuration
-----------------

export CFLAGS="-g -Wall -Wextra"
export LDFLAGS="-Wl,--as-needed -static-libgcc"

./configure --prefix=/home/sergey/lib/orc \
    --disable-shared \
    --enable-static

schroedinger configuration
--------------------------

export CFLAGS="-g -Wall -Wextra -I/home/sergey/lib/orc/include/orc-0.4"
export LDFLAGS="-Wl,--as-needed -static-libgcc -L/home/sergey/lib/orc/lib"
export ORC_CFLAGS="-I/home/sergey/lib/orc/include/orc-0.4"
export ORC_LDFLAGS="-L/home/sergey/lib/orc/lib"

./configure --prefix=/home/sergey/lib/schroedinger \
    --disable-shared \
    --enable-static

x264 configuration
------------------

./configure \
    --prefix=/home/sergey/lib/x264 \
    --extra-cflags="-g -Wall -Wextra" \
    --extra-ldflags="-Wl,--as-needed -static-libgcc" \
    --enable-static \
    --enable-win32thread \
    --disable-lavf \
    --cross-prefix=mingw32- \
    --host=mingw32

xvidcore configuration
----------------------

export CFLAGS="-g -Wall -Wextra"
export LDFLAGS="-Wl,--as-needed -static-libgcc"

./configure \
    --prefix=/home/sergey/lib/xvid

- also used patch for xvid from http://code.google.com/p/mingw-w64-dgn/
