cmake_minimum_required(VERSION 2.8)

project(tst-pro)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall")

if (CMAKE_CROSSCOMPILING)
    link_directories($ENV{CVR_SDK_ROOT}/out/system/lib)
    include_directories($ENV{CVR_SDK_ROOT}/out/system/include)
endif ()

add_executable(tst-pro ltpro.c tst-pro.c)
