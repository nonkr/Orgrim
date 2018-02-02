cmake_minimum_required(VERSION 2.8)

project(c_semaphore)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

add_executable(IPC_PRIVATE IPC_PRIVATE.c)
