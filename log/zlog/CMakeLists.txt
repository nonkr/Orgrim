cmake_minimum_required(VERSION 2.8)

project(test_zlog)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -DORGRIM_DEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

if(CMAKE_CROSSCOMPILING)
    link_directories($ENV{CVR_SDK_ROOT}/out/system/lib)
    include_directories($ENV{CVR_SDK_ROOT}/out/system/include)
endif()

add_definitions(-std=c++11)

add_executable(hello_zlog hello_zlog.cpp)
add_executable(hello_dzlog hello_dzlog.cpp)

target_link_libraries(hello_zlog zlog)
target_link_libraries(hello_dzlog zlog)
