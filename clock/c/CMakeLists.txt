cmake_minimum_required(VERSION 2.8)

project(clock)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -DORGRIM_DEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

if(CMAKE_CROSSCOMPILING)
    link_directories($ENV{CVR_SDK_ROOT}/out/system/lib)
    include_directories($ENV{CVR_SDK_ROOT}/out/system/include)
endif()

add_executable(clock_t clock_t.c)
add_executable(clock_t_macros clock_t_macros.c)

target_link_libraries(clock_t m)
target_link_libraries(clock_t_macros m)
