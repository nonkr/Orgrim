cmake_minimum_required(VERSION 2.8)

project(cpp_csv)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

add_definitions(-std=c++11)

if (CMAKE_CROSSCOMPILING)
    link_directories($ENV{CVR_SDK_ROOT}/out/system/lib)
    include_directories($ENV{CVR_SDK_ROOT}/out/system/include)
endif ()

add_executable(parse_csv test.cpp csv.cpp)
add_executable(calculate_barometer_with_standard calculate_barometer_with_standard.cpp csv.cpp)
add_executable(calculate_recharge_time_cost calculate_recharge_time_cost.cpp csv.cpp)
add_executable(calculate_clean_duration calculate_clean_duration.cpp csv.cpp)
