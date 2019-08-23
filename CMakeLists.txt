cmake_minimum_required(VERSION 3.3.2)
project(Orgrim)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
file(GLOB_RECURSE SOURCE_FILES
        *.c
        *.cpp
        */*.c
        */*.cpp
        )

add_executable(Orgrim ${SOURCE_FILES} datetime/c/chinese_calendar.c)
