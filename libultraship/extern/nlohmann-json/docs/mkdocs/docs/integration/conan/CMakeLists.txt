project(json_example)
cmake_minimum_required(VERSION 3.16...3.31.5)
add_definitions("-std=c++11")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(json_example example.cpp)
target_link_libraries(json_example ${CONAN_LIBS})
