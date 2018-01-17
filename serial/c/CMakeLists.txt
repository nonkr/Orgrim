cmake_minimum_required(VERSION 2.8)

project(serial)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

if(CMAKE_CROSSCOMPILING)
link_directories($ENV{CVR_SDK_ROOT}/out/system/lib)
include_directories(${CMAKE_INSTALL_PREFIX}/include)
endif()

add_executable(serial_test serial_test.c)
add_executable(serial_server serial_server.c)
add_executable(serial_client serial_client.c)

find_package(Threads)
if(CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(serial_test "${CMAKE_THREAD_LIBS_INIT}")
    target_link_libraries(serial_server "${CMAKE_THREAD_LIBS_INIT}")
    target_link_libraries(serial_client "${CMAKE_THREAD_LIBS_INIT}")
endif()

target_link_libraries(serial_client readline ncurses)

install(TARGETS serial_test DESTINATION bin)
