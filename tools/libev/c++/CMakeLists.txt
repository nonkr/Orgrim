cmake_minimum_required(VERSION 2.8)

project(cxx_libev)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall")

if(CMAKE_CROSSCOMPILING)
link_directories($ENV{CVR_SDK_ROOT}/out/system/lib)
include_directories($ENV{CVR_SDK_ROOT}/out/system/include)
endif()

add_executable(socket socket.cpp)
add_executable(custom custom.cpp)

target_link_libraries(socket ev)
