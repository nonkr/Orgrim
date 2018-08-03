cmake_minimum_required(VERSION 2.8)

project(c_socket)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

add_executable(socket_server socket_server.c)
add_executable(socket_client socket_client.c)

find_package(Threads REQUIRED)
target_link_libraries(socket_server "${CMAKE_THREAD_LIBS_INIT}")
