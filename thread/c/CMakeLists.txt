cmake_minimum_required(VERSION 2.8)

project(c_thread)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

add_executable(hello hello.c)
add_executable(hello_arg2 hello_arg2.c)
add_executable(hello_joinable hello_joinable.c)
add_executable(hello_stack hello_stack.c)
add_executable(hello_mutex hello_mutex.c)
add_executable(hello_cond hello_cond.c)
add_executable(simple_threadpool_test simple_threadpool_test.c simple_threadpool.c)

find_package(Threads)
if(CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(hello "${CMAKE_THREAD_LIBS_INIT}")
    target_link_libraries(hello_arg2 "${CMAKE_THREAD_LIBS_INIT}")
    target_link_libraries(hello_joinable "${CMAKE_THREAD_LIBS_INIT}")
    target_link_libraries(hello_stack "${CMAKE_THREAD_LIBS_INIT}")
    target_link_libraries(hello_mutex "${CMAKE_THREAD_LIBS_INIT}")
    target_link_libraries(hello_cond "${CMAKE_THREAD_LIBS_INIT}")
    target_link_libraries(simple_threadpool_test "${CMAKE_THREAD_LIBS_INIT}")
endif()

target_link_libraries(hello_joinable m)
