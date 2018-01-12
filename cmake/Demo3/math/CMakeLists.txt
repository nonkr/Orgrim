# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
aux_source_directory(. DIR_LIB_SRCS)

# 指定生成 MathFunctions 链接库
# 生成静态库，可以不加STATIC，默认就是
add_library(MathFunctions STATIC ${DIR_LIB_SRCS})
# 生成动态库
#add_library(MathFunctions SHARED ${DIR_LIB_SRCS})
