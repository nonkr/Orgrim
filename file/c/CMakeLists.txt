cmake_minimum_required(VERSION 2.8)

project(c_file)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -DORGRIM_DEBUG -Wno-unused-variable")

if(CMAKE_CROSSCOMPILING)
    link_directories($ENV{CVR_SDK_ROOT}/out/system/lib)
    include_directories($ENV{CVR_SDK_ROOT}/out/system/include)
endif()

add_executable(proc_cmdline proc_cmdline.c)
add_executable(read_file_total read_file_total.c)
add_executable(read_file_by_block read_file_by_block.c)
add_executable(read_file_line_by_line read_file_line_by_line.c)
add_executable(txt2bin txt2bin.c)
add_executable(proc_net_route proc_net_route.c)
add_executable(proc_meminfo proc_meminfo.c)
