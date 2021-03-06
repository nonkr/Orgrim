cmake_minimum_required(VERSION 2.8)

project(cpp_thread)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -DORGRIM_DEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -DORGRIM_DEBUG")

add_definitions(-std=c++11)

if (CMAKE_CROSSCOMPILING)
    link_directories($ENV{CVR_SDK_ROOT}/out/system/lib)
    include_directories($ENV{CVR_SDK_ROOT}/out/system/include)
endif ()

#add_executable(test_thread_join test_thread_join.cpp)
#add_executable(thread thread.cpp)
#add_executable(thread2 thread2.cpp)
#add_executable(thread_detach thread_detach.cpp)
#add_executable(thread_swap thread_swap.cpp)
#add_executable(ctpl_example ctpl_example.cpp)
#add_executable(hardware_concurrency hardware_concurrency.cpp)
#add_executable(future_get future/get.cpp)
#add_executable(atomic atomic/atomic.cpp)
#add_executable(producer_consumer_std_thread practice/producer_consumer_std_thread.cpp)
#add_executable(producer_consumer_pthread practice/producer_consumer_pthread.cpp)
#
#add_executable(mutex mutex/mutex.cpp)
#add_executable(recursive_mutex mutex/recursive_mutex.cpp)
add_executable(try_lock_for mutex/try_lock_for.cpp)
add_executable(try_lock_until mutex/try_lock_until.cpp)
add_executable(pthread_mutex_timedlock mutex/pthread_mutex_timedlock.cpp)
#add_executable(lock_guard mutex/lock_guard.cpp)
#add_executable(unique_lock mutex/unique_lock.cpp)
#add_executable(unique_lock2 mutex/unique_lock2.cpp)
#
#add_executable(condition_variable0 condition_variable/condition_variable0.cpp)
#add_executable(condition_variable condition_variable/condition_variable.cpp)
#add_executable(condition_variable2 condition_variable/condition_variable2.cpp)
#add_executable(condition_variable3 condition_variable/condition_variable3.cpp)
#
#add_executable(test_barrier barrier/test_barrier.cpp)

find_package(Threads)
if (CMAKE_THREAD_LIBS_INIT)
#    target_link_libraries(test_thread_join "${CMAKE_THREAD_LIBS_INIT}")
#    target_link_libraries(thread "${CMAKE_THREAD_LIBS_INIT}")
#    target_link_libraries(thread2 "${CMAKE_THREAD_LIBS_INIT}")
#    target_link_libraries(thread_detach "${CMAKE_THREAD_LIBS_INIT}")
#    target_link_libraries(thread_swap "${CMAKE_THREAD_LIBS_INIT}")
#    target_link_libraries(ctpl_example "${CMAKE_THREAD_LIBS_INIT}")
#    target_link_libraries(future_get "${CMAKE_THREAD_LIBS_INIT}")
#    target_link_libraries(atomic "${CMAKE_THREAD_LIBS_INIT}")
#    target_link_libraries(producer_consumer_std_thread "${CMAKE_THREAD_LIBS_INIT}")
#    target_link_libraries(producer_consumer_pthread "${CMAKE_THREAD_LIBS_INIT}")
#
#    target_link_libraries(mutex "${CMAKE_THREAD_LIBS_INIT}")
#    target_link_libraries(recursive_mutex "${CMAKE_THREAD_LIBS_INIT}")
    target_link_libraries(try_lock_for "${CMAKE_THREAD_LIBS_INIT}")
    target_link_libraries(try_lock_until "${CMAKE_THREAD_LIBS_INIT}")
    target_link_libraries(pthread_mutex_timedlock "${CMAKE_THREAD_LIBS_INIT}")
#    target_link_libraries(lock_guard "${CMAKE_THREAD_LIBS_INIT}")
#    target_link_libraries(unique_lock "${CMAKE_THREAD_LIBS_INIT}")
#    target_link_libraries(unique_lock2 "${CMAKE_THREAD_LIBS_INIT}")
#
#    target_link_libraries(condition_variable0 "${CMAKE_THREAD_LIBS_INIT}")
#    target_link_libraries(condition_variable "${CMAKE_THREAD_LIBS_INIT}")
#    target_link_libraries(condition_variable2 "${CMAKE_THREAD_LIBS_INIT}")
#    target_link_libraries(condition_variable3 "${CMAKE_THREAD_LIBS_INIT}")
#
#    target_link_libraries(test_barrier "${CMAKE_THREAD_LIBS_INIT}")
endif ()
