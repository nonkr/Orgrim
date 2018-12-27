cmake_minimum_required(VERSION 2.8)

project(serial)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -DORGRIM_DEBUG")

if (CMAKE_CROSSCOMPILING)
    link_directories($ENV{CVR_SDK_ROOT}/out/system/lib)
    include_directories($ENV{CVR_SDK_ROOT}/out/system/include)
endif ()

add_executable(imu_test imu_test.cpp IMURequester.cpp)

find_package(Threads REQUIRED)
target_link_libraries(imu_test "${CMAKE_THREAD_LIBS_INIT}")
