cmake_minimum_required(VERSION 2.8)

project(c_threadpool)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

aux_source_directory(. DIR_SRCS)

add_executable(sample ${DIR_SRCS})

find_package(Threads REQUIRED)
target_link_libraries(sample "${CMAKE_THREAD_LIBS_INIT}")
