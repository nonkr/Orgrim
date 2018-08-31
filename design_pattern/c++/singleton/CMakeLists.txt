cmake_minimum_required(VERSION 2.8)

project(cxx_design_pattern_singleton)

add_definitions(-std=c++11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

add_executable(EagerSingleton EagerSingleton.cpp)
add_executable(LazySingleton LazySingleton.cpp)
add_executable(MeyersSingleton MeyersSingleton.cpp)
add_executable(LocalStaticSingleton LocalStaticSingleton.cpp)
add_executable(PthreadOnceSingleton PthreadOnceSingleton.cpp)

find_package(Threads REQUIRED)
target_link_libraries(PthreadOnceSingleton "${CMAKE_THREAD_LIBS_INIT}")
