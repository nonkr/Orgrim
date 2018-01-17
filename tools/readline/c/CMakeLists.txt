cmake_minimum_required(VERSION 2.8)

project(readline)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall")

if(CMAKE_CROSSCOMPILING)
link_directories($ENV{CVR_SDK_ROOT}/out/system/lib)
include_directories(${CMAKE_INSTALL_PREFIX}/include)
endif()

aux_source_directory(. DIR_SRCS)

add_executable(rl ${DIR_SRCS})

target_link_libraries(rl readline ncurses)

install(TARGETS rl DESTINATION bin)
