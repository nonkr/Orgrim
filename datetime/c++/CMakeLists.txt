cmake_minimum_required(VERSION 2.8)

project(datetime_cxx)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -DORGRIM_DEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

add_definitions(-D_GNU_SOURCE -DLINUX -DENABLE_ASSERT -DHAVE_SYS_UIO_H)
add_definitions(-DSUPPORT_ION -DUSE_RK_V4L2_HEAD_FILES -DRK_ISP11 -D_FILE_OFFSET_BITS=64 -DMARKUP_STDCONV)

if(CMAKE_CROSSCOMPILING)
    link_directories($ENV{CVR_SDK_ROOT}/out/system/lib)
    include_directories($ENV{CVR_SDK_ROOT}/out/system/include)
endif()

add_definitions(-std=c++11)

include_directories(${PROJECT_SOURCE_DIR}/../../common)

add_executable(strftime strftime.cpp)
add_executable(system_clock system_clock.cpp)

target_link_libraries(system_clock log)
