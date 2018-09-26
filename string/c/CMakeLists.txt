cmake_minimum_required(VERSION 2.8)

project(serial)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

if (CMAKE_CROSSCOMPILING)
    link_directories($ENV{CVR_SDK_ROOT}/out/system/lib)
    include_directories($ENV{CVR_SDK_ROOT}/out/system/include)
endif ()

add_executable(convert_hexstring_to_buffer convert_hexstring_to_buffer.c)
add_executable(convert_hexstring_to_buffer2 convert_hexstring_to_buffer2.c)
