cmake_minimum_required(VERSION 2.8)

project(cpp_memorypool)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

add_definitions(-std=c++11)

if (CMAKE_CROSSCOMPILING)
    link_directories($ENV{CVR_SDK_ROOT}/out/system/lib)
    include_directories($ENV{CVR_SDK_ROOT}/out/system/include)
endif ()

add_executable(pure_benchmark benchmark.cpp)

find_package(Threads REQUIRED)
target_link_libraries(pure_benchmark "${CMAKE_THREAD_LIBS_INIT}")
