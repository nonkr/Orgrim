cmake_minimum_required(VERSION 2.8)

project(c_io_multiplexing)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

if (CMAKE_CROSSCOMPILING)
    link_directories($ENV{CVR_SDK_ROOT}/out/system/lib)
    include_directories($ENV{CVR_SDK_ROOT}/out/system/include)
endif ()

add_executable(select select.c)
add_executable(poll poll.c)
add_executable(epoll epoll.c)
add_executable(namedpipe_client namedpipe_client.c)
