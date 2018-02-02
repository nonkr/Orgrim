cmake_minimum_required(VERSION 2.8)

project(c_semaphore)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

add_executable(system_v_sem system_v_sem.c)
add_executable(posix_sem posix_sem.c)

find_package(Threads)
if(CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(posix_sem "${CMAKE_THREAD_LIBS_INIT}")
endif()