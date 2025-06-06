
cmake_minimum_required(VERSION 3.10)
project(starship-android)

# libultraship submodule path
set(LUS_ROOT ${CMAKE_SOURCE_DIR}/../../libultraship)
add_subdirectory(${LUS_ROOT} libultraship_build)

# Starship native code
set(STARSHIP_SRC_DIR ${CMAKE_SOURCE_DIR}/../../src)
file(GLOB_RECURSE STARSHIP_SOURCES ${STARSHIP_SRC_DIR}/*.c ${STARSHIP_SRC_DIR}/*.cpp)

add_library(starship SHARED src/main/jni/native-lib.cpp ${STARSHIP_SOURCES})
target_include_directories(starship PRIVATE ${STARSHIP_SRC_DIR} ${LUS_ROOT}/src)
target_link_libraries(starship ultraship android log)
