cmake_minimum_required(VERSION 2.8)

project(c_print)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

add_executable(print_as_binary print_as_binary.c)
