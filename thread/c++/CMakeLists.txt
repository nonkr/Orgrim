cmake_minimum_required(VERSION 2.8)

project(cpp_thread)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -DORGRIM_DEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

add_definitions(-std=c++11)

add_executable(thread thread.cpp)

find_package(Threads)
if(CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(thread "${CMAKE_THREAD_LIBS_INIT}")
endif()