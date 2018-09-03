### How to install tcmalloc

* 1. download gperftools
* 2. ./configure --disable-cpu-profiler --disable-heap-profiler --disable-heap-checker --disable-debugalloc --enable-minimal
* 3. make && make install

to build for arm-linux
./configure --disable-cpu-profiler --disable-heap-profiler --disable-heap-checker --disable-debugalloc --enable-minimal --prefix=/opt/arm-tcmalloc --host arm-linux CXX=arm-linux-g++ CC=arm-linux-gcc
make && make install
