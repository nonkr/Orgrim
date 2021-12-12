cmake_minimum_required(VERSION 2.8)

project(c_timer)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

add_executable(alarm alarm.c)
add_executable(timerfd timerfd.c)
