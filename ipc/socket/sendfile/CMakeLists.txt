cmake_minimum_required(VERSION 2.8)

project(c_sendfile)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

add_executable(sendfile_server sendfile_server.c)
add_executable(sendfile_client sendfile_client.c)

find_package(Threads REQUIRED)
target_link_libraries(sendfile_server "${CMAKE_THREAD_LIBS_INIT}")
