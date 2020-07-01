cmake_minimum_required(VERSION 2.8)

project(test_memleak)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -fsanitize=address -g -fno-omit-frame-pointer")

if (CMAKE_CROSSCOMPILING)
    link_directories($ENV{CVR_SDK_ROOT}/out/system/lib)
    include_directories($ENV{CVR_SDK_ROOT}/out/system/include)
endif ()

#add_definitions(-D_GNU_SOURCE -DLINUX -DENABLE_ASSERT -DHAVE_SYS_UIO_H)
#add_definitions(-DSUPPORT_ION -DUSE_RK_V4L2_HEAD_FILES -DRK_ISP11 -D_FILE_OFFSET_BITS=64 -DMARKUP_STDCONV)

add_executable(memory_leak memory_leak.cpp)
add_executable(use_after_free use_after_free.cpp)
add_executable(heap_buffer_overflow heap_buffer_overflow.cpp)
add_executable(stack_buffer_overflow stack_buffer_overflow.cpp)
add_executable(global_buffer_overflow global_buffer_overflow.cpp)
add_executable(stack_use_after_scope stack_use_after_scope.cpp)
add_executable(stack_use_after_return stack_use_after_return.cpp)
