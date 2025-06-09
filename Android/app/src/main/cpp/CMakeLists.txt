cmake_minimum_required(VERSION 3.10.2)

project(starship-android)

add_subdirectory(${CMAKE_SOURCE_DIR}/../../../libultraship ${CMAKE_BINARY_DIR}/libultraship)

add_library(native-lib SHARED native-lib.cpp)

target_include_directories(native-lib PRIVATE
    ${CMAKE_SOURCE_DIR}/../../../libultraship
)

target_link_libraries(native-lib
    ultralib
    android
    EGL
    GLESv2
    log
    OpenSLES
)
