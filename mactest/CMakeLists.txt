#cmake -DCMAKE_TOOLCHAIN_FILE=$CVR_SDK_ROOT/build/toolchain-arm-linux.cmake ..
cmake_minimum_required(VERSION 2.8)

project(mactest)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -O2 -fPIC -DORGRIM_DEBUG")
# strip
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s")

#link_directories(${PROJECT_SOURCE_DIR}/lib)
include_directories(${PROJECT_SOURCE_DIR}/)
link_directories(${PROJECT_SOURCE_DIR}/)

if (CMAKE_CROSSCOMPILING)
    link_directories($ENV{CVR_SDK_ROOT}/out/system/lib)
    include_directories($ENV{CVR_SDK_ROOT}/out/system/include)
endif ()

add_definitions(-D_GNU_SOURCE -DLINUX -DENABLE_ASSERT)
add_definitions(-DSUPPORT_ION -DUSE_RK_V4L2_HEAD_FILES -DRK_ISP11 -D_FILE_OFFSET_BITS=64 -DMARKUP_STDCONV)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SRCS main.cpp
         MPToF.cpp
         ionbuffer.c)

add_executable(mactest ${SRCS})

#target_link_libraries(mactest ion avformat avcodec avutil mpp salsa fdk-aac cam_hal vpu fsmanage rkrga)
target_link_libraries(mactest ion tfc2)
