# CMake 最低版本号要求
cmake_minimum_required(VERSION 2.8)

# 项目信息
project(Demo3_1)

# 查找目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)

# 1. 方式一
#FIND_LIBRARY(MYMATH_LIB MathFunctions math/build NO_DEFAULT_PATH)
#link_libraries(${MYMATH_LIB})

# 2.1 方式二
# 添加链接库
link_directories(${PROJECT_SOURCE_DIR}/math/build)

# 指定生成目标
add_executable(Demo ${DIR_SRCS})

# 2.2 方式二
# 添加链接库
target_link_libraries(Demo MathFunctions)
