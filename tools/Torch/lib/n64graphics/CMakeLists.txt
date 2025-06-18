cmake_minimum_required(VERSION 3.12)
project(N64Graphics)

set(CMAKE_CXX_STANDARD 20)

file(GLOB CXX_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
set(SRC_DIR ${CXX_FILES})

add_library(${PROJECT_NAME} STATIC ${SRC_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})