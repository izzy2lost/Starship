cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)
project(Starship VERSION 2.0.0 LANGUAGES C CXX)

# Android-specific setup
if(ANDROID)
    set(ANDROID_STL c++_shared)
    set(ANDROID_PLATFORM android-24)
    add_definitions(-DANDROID -DPLATFORM_ANDROID=1)
    find_package(OpenGLES REQUIRED)
endif()

# ... [Common setup: version, phonetic alphabet, etc.] ...

# Include directories (common + Android-specific)
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    $<IF:$<BOOL:${ANDROID}>,${OPENGLES_INCLUDE_DIR},>
)

# Source files (same as original)
file(GLOB_RECURSE ALL_FILES ...)
list(FILTER ALL_FILES EXCLUDE ...)

# Build target (shared lib for Android)
if(ANDROID)
    add_library(${PROJECT_NAME} SHARED ${ALL_FILES})
else()
    add_executable(${PROJECT_NAME} ${ALL_FILES})
endif()

# Libultraship integration (common)
add_subdirectory(libultraship)
target_link_libraries(${PROJECT_NAME} PRIVATE libultraship)

# Android-specific linking
if(ANDROID)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        OpenGLES::GLESv2
        log
        android
    )
    
    # Copy assets post-build
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets/"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets/"
    )
endif()

# ... [Common compiler flags, definitions, etc.] ...

# Android compiler adjustments
if(ANDROID)
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall
        -Wno-unused-parameter
        -fpermissive
    )
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()
