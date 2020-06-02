#!/usr/bin/env bash

g++ initialization_order_bug1.cpp initialization_order_bug2.cpp && ./a.out
# 1
g++ initialization_order_bug2.cpp initialization_order_bug1.cpp && ./a.out
# 43

g++ initialization_order_bug1.cpp initialization_order_bug2.cpp -fsanitize=address -g
ASAN_OPTIONS=check_initialization_order=true:strict_init_order=true ./a.out
