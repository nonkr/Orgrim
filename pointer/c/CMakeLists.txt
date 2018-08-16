cmake_minimum_required(VERSION 2.8)

project(c_pointer)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -DORGRIM_DEBUG")

add_executable(struct_array_pointer struct_array_pointer.c)
add_executable(integer_pointer integer_pointer.c)
