# 1. 声明cmake所需版本
cmake_minimum_required(VERSION 3.1.3)
# 2. 设置工程项目名称（内部会生成一系列的内置变量）
project(video_server VERSION 1.0)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++17")
# 4. 执行外部命令，生成临时代码文件: cal.pb.h cal.pb.cc
set(data_dir ${CMAKE_CURRENT_SOURCE_DIR}/../data)
set(proto_dir ${CMAKE_CURRENT_SOURCE_DIR}/../proto)
set(comm_dir ${CMAKE_CURRENT_SOURCE_DIR}/../common)
# 3. 添加生成目标（可执行程序）
file(GLOB_RECURSE src_files ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cc)
add_executable(${PROJECT_NAME} ${src_files})

# 5. 头文件路径设置
include_directories(${comm_dir})
include_directories(${data_dir})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
# 6. 依赖检查（依赖库的查找以及链接选项的添加）
find_package(bite_scaffold REQUIRED)
# 7. 设置链接库
target_link_libraries(${PROJECT_NAME} PRIVATE bite_scaffold::bite_scaffold videoplayer)
INSTALL(TARGETS ${PROJECT_NAME} DESTINATION bin)
INSTALL(TARGETS ${PROJECT_NAME} DESTINATION bin)