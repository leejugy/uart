source /opt/gcc-beaglebone/environment-setup-armv7at2hf-neon-poky-linux-gnueabi
export SCR="main.c uart.c uart.h"
export OBJECT="main"
export LDFLAGS="-Wunused"
export CFLAGS="-lrt -lpthread"
export CPPFLAGS=""
make -j
