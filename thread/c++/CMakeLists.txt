cmake_minimum_required(VERSION 2.8)

project(cpp_thread)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -DORGRIM_DEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

add_definitions(-std=c++11)

add_executable(thread thread.cpp)
add_executable(thread2 thread2.cpp)
add_executable(thread_detach thread_detach.cpp)
add_executable(thread_swap thread_swap.cpp)
add_executable(ctpl_example ctpl_example.cpp)

find_package(Threads)
if(CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(thread "${CMAKE_THREAD_LIBS_INIT}")
    target_link_libraries(thread2 "${CMAKE_THREAD_LIBS_INIT}")
    target_link_libraries(thread_detach "${CMAKE_THREAD_LIBS_INIT}")
    target_link_libraries(thread_swap "${CMAKE_THREAD_LIBS_INIT}")
    target_link_libraries(ctpl_example "${CMAKE_THREAD_LIBS_INIT}")
endif()