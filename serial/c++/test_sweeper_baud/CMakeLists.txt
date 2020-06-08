cmake_minimum_required(VERSION 2.8)

project(test_sweeper_baud)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -O2 -fPIC -Wno-unused-function -Wno-unused-variable -Wno-literal-suffix")

if (CMAKE_CROSSCOMPILING)
    link_directories($ENV{CVR_SDK_ROOT}/out/system/lib)
    include_directories($ENV{CVR_SDK_ROOT}/out/system/include)
endif ()

add_definitions(-D_GNU_SOURCE -DLINUX -DENABLE_ASSERT -DHAVE_SYS_UIO_H)
add_definitions(-DSUPPORT_ION -DUSE_RK_V4L2_HEAD_FILES -DRK_ISP11 -D_FILE_OFFSET_BITS=64 -DMARKUP_STDCONV)

add_executable(test_sweeper_baud main.cpp McuComm.cpp SerialPort.cpp)

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} "${CMAKE_THREAD_LIBS_INIT}")
target_link_libraries(${PROJECT_NAME} log)
