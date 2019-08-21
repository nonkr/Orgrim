########################################################################################################################
#
# Build Note
#
# cmake options:
#   -DCMAKE_TOOLCHAIN_FILE=$CVR_SDK_ROOT/build/toolchain-arm-linux.cmake
#
# $ mkdir build
# $ cd build
# $ cmake -DCMAKE_TOOLCHAIN_FILE=$CVR_SDK_ROOT/build/toolchain-arm-linux.cmake ..
# $ make
#
########################################################################################################################

cmake_minimum_required(VERSION 2.8)

project(net_server)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -O2 -fPIC -Wno-unused-function -Wno-unused-variable -Wno-literal-suffix")

# add -g in release version
# add -s in debug version
string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
if (NOT CMAKE_BUILD_TYPE_LOWER OR CMAKE_BUILD_TYPE_LOWER STREQUAL "release")
    message("Build in release mode")
    # strip
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s")
elseif (CMAKE_BUILD_TYPE_LOWER STREQUAL "debug")
    message("Build in debug mode")
endif ()

if (CMAKE_CROSSCOMPILING)
    link_directories($ENV{CVR_SDK_ROOT}/out/system/lib)
    include_directories($ENV{CVR_SDK_ROOT}/out/system/include)
endif ()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# application source files
file(GLOB_RECURSE SOURCE_FILES
        main.cpp
        CmdOptions.cpp
        NetServer.cpp
        ServerApp.cpp
        )

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# find pthread library and then link it
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} "${CMAKE_THREAD_LIBS_INIT}")
