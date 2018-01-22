cmake_minimum_required(VERSION 2.8)

project(c_file)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

add_executable(proc_cmdline proc_cmdline.c)
