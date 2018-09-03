### How to install jemalloc

* 1. download jemalloc-5.1.0
* 2. ./autogen.sh
* 3. make && make install

to build for arm-linux
./configure --prefix=/opt/arm-jemalloc --host arm-linux CXX=arm-linux-g++ CC=arm-linux-gcc
make && make install
