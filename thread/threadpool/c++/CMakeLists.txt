cmake_minimum_required(VERSION 2.8)

project(cxx_threadpool)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -DORGRIM_DEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -DORGRIM_DEBUG -std=c++11")

aux_source_directory(. DIR_SRCS)

add_executable(sample ${DIR_SRCS})

find_package(Threads REQUIRED)
target_link_libraries(sample "${CMAKE_THREAD_LIBS_INIT}")
