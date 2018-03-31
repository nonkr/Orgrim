cmake_minimum_required(VERSION 2.8)

project(c_socket)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -DORGRIM_DEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

add_executable(simple_client simple_client.c)
add_executable(simple_server simple_server.c)
