cmake_minimum_required(VERSION 2.8)

project(ping_cxx)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")

if(CMAKE_CROSSCOMPILING)
    link_directories($ENV{CVR_SDK_ROOT}/out/system/lib)
    include_directories($ENV{CVR_SDK_ROOT}/out/system/include)
endif()

add_executable(ping main.cpp PingUtil.cpp)

target_link_libraries(ping m)

find_package(Threads REQUIRED)
target_link_libraries(ping "${CMAKE_THREAD_LIBS_INIT}")
