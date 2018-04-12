cmake_minimum_required(VERSION 2.8)

# Options. Turn on with 'cmake -Dmyvarname=ON'.
option(test "Build all tests." OFF) # Makes boolean 'test' available.

project(cxx_gtest_sample2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall")

add_executable(app main.cpp sample.cpp)

if (test)
    enable_testing()

    ##############
    # Unit Tests
    ##############
    add_executable(runUnitTests test.cpp sample.cpp)

    # Standard linking to gtest stuff.
    target_link_libraries(runUnitTests gtest gtest_main)

    # This is so you can do 'make test' to see all your tests run, instead of
    # manually running the executable runUnitTests to see those specific tests.
    add_test(NAME that-test-I-made COMMAND runUnitTests)
    # or
    # You can also omit NAME and COMMAND. The second argument could be some other
    # test executable.
    add_test(that-other-test-I-made runUnitTests)
endif ()
