cmake_minimum_required(VERSION 2.8)

project(c_barrier)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

add_executable(sample sample.c)

find_package(Threads)
if(CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(sample "${CMAKE_THREAD_LIBS_INIT}")
endif()
