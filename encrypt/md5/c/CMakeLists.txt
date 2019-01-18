cmake_minimum_required(VERSION 2.8)

project(c_md5)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -DORGRIM_DEBUG")

add_executable(md5_sample1 md5_sample1.c)
add_executable(md5_sample2 md5_sample2.c)
add_executable(md5_daemon md5_daemon.c)
add_executable(hmac_md5 hmac_md5.c)

target_link_libraries(md5_sample1 crypto)
target_link_libraries(md5_sample2 crypto)
target_link_libraries(md5_daemon crypto)
target_link_libraries(hmac_md5 crypto)
