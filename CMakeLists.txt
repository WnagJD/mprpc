#cmake的最低版本
cmake_minimum_required(VERSION 3.0)

project(mprpc)

set(CMAKE_BUILD_TYPE "Debug")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)


#配置头文件加载路径
include_directories(${PROJECT_SOURCE_DIR}/example)
include_directories(${PROJECT_SOURCE_DIR}/src/include)

#配置库文件加载路径
link_directories(${PROJECT_SOURCE_DIR}/lib)


add_subdirectory(src)
add_subdirectory(example)


