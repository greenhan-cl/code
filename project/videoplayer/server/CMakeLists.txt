# 编写一个总的cmake构建规则,针对各个子服务进行构建
#   1. 针对系统的中的公共代码进行编译并打包成库,提供给各个子服务使用(避免了每个子服务都需要将公共代码编译一遍的时间消耗)
#   2. 在针对各个子服务进行各自程序的编译

cmake_minimum_required(VERSION 3.1.3)
project(videoplayer VERSION 1.0)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++17")

set(odb_files ${CMAKE_CURRENT_SOURCE_DIR}/data/data.h)
file(GLOB_RECURSE proto_files ${CMAKE_CURRENT_SOURCE_DIR}/proto/*.proto)
execute_process(
    COMMAND odb -d mysql --std c++17 --generate-query --generate-schema --profile boost/date-time ${odb_files}
    COMMAND protoc --experimental_allow_proto3_optional -I ${CMAKE_CURRENT_SOURCE_DIR}/proto --cpp_out=${CMAKE_CURRENT_BINARY_DIR} ${proto_files}
)

file(GLOB_RECURSE common_files 
    ${CMAKE_CURRENT_SOURCE_DIR}/data/*.cc  
    ${CMAKE_CURRENT_SOURCE_DIR}/common/*.cc
    ${CMAKE_CURRENT_BINARY_DIR}/*.cc 
    ${CMAKE_CURRENT_BINARY_DIR}/*.cxx)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/data)
include_directories(${CMAKE_CURRENT_BINARY_DIR})


# 设定要生成的库目标
add_library(${PROJECT_NAME} STATIC ${common_files})
set_target_properties(${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
target_link_libraries(${PROJECT_NAME} PRIVATE bite_scaffold::bite_scaffold)

find_package(bite_scaffold REQUIRED)

link_directories(${CMAKE_CURRENT_BINARY_DIR}/lib)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/svc_file)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/svc_user)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/svc_video)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/svc_transcode)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/svc_gateway)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR})