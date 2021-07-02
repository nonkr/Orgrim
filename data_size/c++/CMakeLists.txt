cmake_minimum_required(VERSION 2.8)

project(cpp_datasize)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -DORGRIM_DEBUG -Wno-unused-but-set-variable")

add_definitions(-std=c++11)

if (CMAKE_CROSSCOMPILING)
    link_directories($ENV{CVR_SDK_ROOT}/out/system/lib)
    include_directories($ENV{CVR_SDK_ROOT}/out/system/include)
else()
    add_executable(data_size_32 data_size.cpp)
    set_target_properties(data_size_32 PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
endif ()

add_executable(data_size_64 data_size.cpp)
