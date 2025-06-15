cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(starship_android LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

# Use OPENGLES for Android
option(USE_OPENGLES "Enable GLES3 (OpenGL ES)" ON)

# Global Compile Definitions
add_compile_definitions(
    VERSION_US=1
    ENABLE_RUMBLE=1
    F3DEX_GBI=1
    _LANGUAGE_C
    _USE_MATH_DEFINES
    CIMGUI_DEFINE_ENUMS_AND_STRUCTS
    NON_MATCHING=1
    NON_EQUIVALENT=1
    AVOID_UB=1
    GBI_FLOATS=1
    # For Android-specific disables:
    ANDROID
)

# --- Smart Source Collection: all .c/.cpp but skip 3rdparty, test, build, etc ---
file(GLOB_RECURSE STARSHIP_SOURCES
    "src/*.c"
    "src/*.cpp"
    "libultraship/src/*.c"
    "libultraship/src/*.cpp"
)

foreach(file ${STARSHIP_SOURCES})
    if (
        file MATCHES "/third[_-]?party/"
        OR file MATCHES "/test/"
        OR file MATCHES "/build/"
        OR file MATCHES "/\\.git/"
        OR file MATCHES "/docs/"
        OR file MATCHES "~$"
    )
        list(REMOVE_ITEM STARSHIP_SOURCES "${file}")
    endif()
endforeach()

# --- Smart Includes (all relevant headers from main repo and libultraship) ---
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/libultraship/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libultraship/include/libultraship
    ${CMAKE_CURRENT_SOURCE_DIR}/libultraship/src
    ${CMAKE_CURRENT_SOURCE_DIR}/libultraship/src/log
    ${CMAKE_CURRENT_SOURCE_DIR}/libultraship/src/debug
    ${CMAKE_CURRENT_SOURCE_DIR}/libultraship/src/menu
    ${CMAKE_CURRENT_SOURCE_DIR}/libultraship/src/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/libultraship/src/utils/binarytools
    ${CMAKE_CURRENT_SOURCE_DIR}/libultraship/src/config
    ${CMAKE_CURRENT_SOURCE_DIR}/libultraship/src/resource
    ${CMAKE_CURRENT_SOURCE_DIR}/libultraship/src/resource/type
    ${CMAKE_CURRENT_SOURCE_DIR}/libultraship/src/resource/factory
    ${CMAKE_CURRENT_SOURCE_DIR}/libultraship/src/audio
    ${CMAKE_CURRENT_SOURCE_DIR}/libultraship/src/window
    ${CMAKE_CURRENT_SOURCE_DIR}/libultraship/src/window/gui
    ${CMAKE_CURRENT_SOURCE_DIR}/libultraship/src/public
    ${CMAKE_CURRENT_SOURCE_DIR}/libultraship/src/public/libultra
    ${CMAKE_CURRENT_SOURCE_DIR}/libultraship/src/public/bridge
    ${CMAKE_CURRENT_SOURCE_DIR}/libultraship/extern
    ${CMAKE_CURRENT_SOURCE_DIR}/libultraship/extern/tinyxml2
    ${CMAKE_CURRENT_SOURCE_DIR}/libultraship/libultraship/Lib/
    ${CMAKE_CURRENT_SOURCE_DIR}/libultraship/libultraship/Lib/libjpeg/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/libultraship/libultraship/Lib/spdlog/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/libultraship/src/graphic/Fast3D/U64/PR
    ${CMAKE_CURRENT_SOURCE_DIR}/libultraship/src/graphic
)

# --- Library Target (STATIC or SHARED as needed by your gradle) ---
add_library(starship SHARED ${STARSHIP_SOURCES})
set_target_properties(starship PROPERTIES PREFIX "")

# --- Android-specific linking: logcat and GLES ---
if (CMAKE_SYSTEM_NAME STREQUAL "Android")
    find_library(ANDROID_LOG_LIB log)
    if (ANDROID_LOG_LIB)
        target_link_libraries(starship PRIVATE ${ANDROID_LOG_LIB})
    endif()
    if(USE_OPENGLES)
        find_library(OPENGLES_LIBRARY GLESv3)
        if(OPENGLES_LIBRARY)
            target_link_libraries(starship PRIVATE ${OPENGLES_LIBRARY})
        endif()
    endif()
endif()

# --- Compiler options: warnings, optimizations ---
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(starship PRIVATE
        -Wall -Wextra -Wno-unused-parameter -Wno-unused-function
        -Wno-missing-field-initializers -Wno-narrowing -Wno-missing-braces
        -Wno-int-conversion -fpermissive
        $<$<COMPILE_LANGUAGE:C>:-Werror-implicit-function-declaration>
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-deprecated-enum-enum-conversion>
    )
endif()

# Optional: If you need resource copying (assets, config.yml) add a custom command/target here

# Optionally export headers for Java/JNI
target_include_directories(starship PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Optionally set more definitions (like JNI, SDL, etc.) if needed
