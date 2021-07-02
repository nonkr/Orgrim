cmake_minimum_required(VERSION 2.8)

project(c_math)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -DORGRIM_DEBUG")

if (CMAKE_CROSSCOMPILING)
    link_directories($ENV{CVR_SDK_ROOT}/out/system/lib)
    include_directories($ENV{CVR_SDK_ROOT}/out/system/include)
endif ()

add_executable(ceil ceil.c)
add_executable(rand rand.c)

target_link_libraries(ceil m)
