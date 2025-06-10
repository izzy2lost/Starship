cmake_minimum_required(VERSION 3.22.1)
project("starship-android")

# Set Starship source directory
set(STARSHIP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../..)

# Include libultraship
find_package(libultraship REQUIRED CONFIG)

# Add Starship source
add_subdirectory(${STARSHIP_SRC_DIR} starship)

# Android native library
add_library(starship-android SHARED
    android_main.cpp
    # Add other Android-specific files here
)

target_include_directories(starship-android PRIVATE
    ${STARSHIP_SRC_DIR}/src
    ${STARSHIP_SRC_DIR}/soh
)

target_link_libraries(starship-android PRIVATE
    starship
    libultraship::libultraship
    android
    EGL
    GLESv3
    log
)