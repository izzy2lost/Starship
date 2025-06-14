cmake_minimum_required(VERSION 3.16...3.31 FATAL_ERROR)

# Main Project Info
project(Starship VERSION 2.0.0 LANGUAGES C CXX ASM)
include(FetchContent)

# Android detection and setup
if(ANDROID)
    set(PLATFORM_ANDROID 1)
    add_compile_definitions(PLATFORM_ANDROID=1 ANDROID=1 MOBILE_DEVICE)
    message(STATUS "Building for Android platform")
endif()

# Version "codename" logic
set(NATO_PHONETIC_ALPHABET
  Alfa Bravo Charlie Delta Echo Foxtrot Golf Hotel
  India Juliett Kilo Lima Mike November Oscar Papa
  Quebec Romeo Sierra Tango Uniform Victor Whiskey
  Xray Yankee Zulu
)
math(EXPR PATCH_INDEX "${PROJECT_VERSION_PATCH}")
list(GET NATO_PHONETIC_ALPHABET ${PATCH_INDEX} PROJECT_PATCH_WORD)
set(PROJECT_BUILD_NAME "Barnard ${PROJECT_PATCH_WORD}" CACHE STRING "" FORCE)
set(PROJECT_TEAM "SonicDcer & Lywx" CACHE STRING "" FORCE)

# Set platform defaults
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

# CMake Modules/Defines
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/cmake/modules/")
set(YAML_CPP_STATIC_DEFINE ON)
add_compile_definitions(YAML_CPP_STATIC_DEFINE)

# Android-specific configuration
if(ANDROID)
    # Find essential Android libraries
    find_library(OPENSLES_LIB OpenSLES)
    find_library(ANDROID_LIB android)
    find_library(LOG_LIB log)
    find_library(GLESV2_LIB GLESv2)
    find_library(EGL_LIB EGL)
    
    set(ANDROID_LIBS 
        ${OPENSLES_LIB}
        ${ANDROID_LIB}
        ${LOG_LIB}
        ${GLESV2_LIB}
        ${EGL_LIB}
    )
    
    # NDK-specific includes
    include_directories(
        ${ANDROID_NDK}/sources/android/native_app_glue
        ${ANDROID_NDK}/sources/cxx-stl/llvm-libc++/include
    )
    
    # Compiler flags for Android
    add_compile_options(
        -fPIC
        -ffunction-sections
        -funwind-tables
        -fstack-protector-strong
        -no-canonical-prefixes
        -Wno-invalid-command-line-argument
        -Wno-unused-command-line-argument
    )
    
    set(CMAKE_SHARED_LINKER_FLAGS 
        "-Wl,--build-id=sha1 -Wl,--no-undefined -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now")
endif()

# Preprocessor definitions for all platforms
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
    $<$<BOOL:${ANDROID}>:__ANDROID__>
)

# Set compile flags for release/debug
if(ANDROID)
    set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")
    set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
elseif(CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
    # ... (keep Switch config) ...
else()
    set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
endif()

# Build version, networking
set(VERSION us)
set(USE_NETWORKING OFF)
set(SKIP_XCODE_VERSION_CHECK ON)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/properties.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/properties.h" @ONLY)

# Project include directories
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# Gather source files
file(GLOB_RECURSE ALL_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "src/audio/*.c"      "src/audio/*.h"
    "src/engine/*.c"     "src/engine/*.h"
    "src/libultra/io/aisetfreq.c"
    "src/libultra/gu/sqrtf.c"
    "src/libultra/gu/mtxutil.c"
    "src/libultra/gu/ortho.c"
    "src/overlays/*.c"   "src/overlays/*.h"
    "src/port/*.h"       "src/port/*.c"   "src/port/*.cpp"
    "src/sys/*.c"        "src/sys/*.h"
    "src/1FA60.c"
    "src/buffers.c"
    "src/dmatable.c"
    "src/libc_math64.c"
    "src/libc_sprintf.c"
)

# Exclude specific files
list(FILTER ALL_FILES EXCLUDE REGEX ".*\\.inc\\.c$")
list(FILTER ALL_FILES EXCLUDE REGEX "src/engine/fox_colheaders\\.c$")
list(FILTER ALL_FILES EXCLUDE REGEX "src/engine/fox_edata_info\\.c$")
list(FILTER ALL_FILES EXCLUDE REGEX "src/engine/fox_rcp_setup\\.c$")
list(FILTER ALL_FILES EXCLUDE REGEX "src/engine/fox_load_inits\\.c$")
list(FILTER ALL_FILES EXCLUDE REGEX "src/overlays/ovl_ending/fox_end2_data\\.c$")
list(FILTER ALL_FILES EXCLUDE REGEX "src/sys/sys_timer\\.c$")
list(FILTER ALL_FILES EXCLUDE REGEX "src/sys/sys_fault\\.c$")
list(FILTER ALL_FILES EXCLUDE REGEX "src/mods/object_ram\\.c$")

# Always exclude extractor for Android
if(ANDROID OR CMAKE_SYSTEM_NAME STREQUAL "iOS" OR CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
    list(FILTER ALL_FILES EXCLUDE REGEX "src/port/extractor/.*")
endif()

# Android: Build as shared library
if(ANDROID)
    add_library(${PROJECT_NAME} SHARED ${ALL_FILES})
    
    # Add native app glue
    add_library(native_app_glue STATIC
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
    )
    target_link_libraries(${PROJECT_NAME} native_app_glue)
    
    # Android Manifest generation
    configure_file(
        "${CMAKE_SOURCE_DIR}/android/AndroidManifest.xml.in"
        "${CMAKE_BINARY_DIR}/AndroidManifest.xml"
        @ONLY
    )
else()
    add_executable(${PROJECT_NAME} ${ALL_FILES})
endif()

#==============================================================================#
# Libultraship Integration (Android-specific path)                             #
#==============================================================================#
if(ANDROID)
    set(LIBUTRASHIP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android/app/jni/src/libultraship")
else()
    set(LIBUTRASHIP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libultraship")
endif()

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${LIBUTRASHIP_DIR}/include"
    "${LIBUTRASHIP_DIR}/include/libultraship"
    "${LIBUTRASHIP_DIR}/src"
    "${LIBUTRASHIP_DIR}/src/log"
    "${LIBUTRASHIP_DIR}/src/debug"
    "${LIBUTRASHIP_DIR}/src/menu"
    "${LIBUTRASHIP_DIR}/src/utils"
    "${LIBUTRASHIP_DIR}/src/utils/binarytools"
    "${LIBUTRASHIP_DIR}/src/config"
    "${LIBUTRASHIP_DIR}/src/resource"
    "${LIBUTRASHIP_DIR}/src/resource/type"
    "${LIBUTRASHIP_DIR}/src/resource/factory"
    "${LIBUTRASHIP_DIR}/src/audio"
    "${LIBUTRASHIP_DIR}/src/window"
    "${LIBUTRASHIP_DIR}/src/window/gui"
    "${LIBUTRASHIP_DIR}/src/public"
    "${LIBUTRASHIP_DIR}/src/public/libultra"
    "${LIBUTRASHIP_DIR}/src/public/bridge"
    "${LIBUTRASHIP_DIR}/extern"
    "${LIBUTRASHIP_DIR}/extern/tinyxml2"
    "${LIBUTRASHIP_DIR}/libultraship/Lib/"
    "${LIBUTRASHIP_DIR}/libultraship/Lib/libjpeg/include/"
    "${LIBUTRASHIP_DIR}/libultraship/Lib/spdlog/include/"
    "${LIBUTRASHIP_DIR}/src/graphic/Fast3D/U64/PR"
    "${LIBUTRASHIP_DIR}/src/graphic"
)

add_subdirectory("${LIBUTRASHIP_DIR}" "${CMAKE_BINARY_DIR}/libultraship")
add_dependencies(${PROJECT_NAME} libultraship)

# Android: Link libraries
if(ANDROID)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        libultraship
        ${ANDROID_LIBS}
        SDL2
        # Add other Android-specific libraries as needed
    )
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE libultraship)
endif()

# Compiler flags for non-MSVC compilers
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    if(ANDROID)
        target_compile_options(${PROJECT_NAME} PRIVATE
            -Wall
            -Wno-unused-parameter
            -Wno-unused-function
            -Wno-unused-variable
            -Wno-missing-field-initializers
            -Wno-parentheses
            -Wno-narrowing
            -Wno-missing-braces
            -Wno-int-conversion
            $<$<COMPILE_LANGUAGE:C>:
                -Wno-incompatible-pointer-types
            >
        )
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin" OR CMAKE_SYSTEM_NAME STREQUAL "iOS")
        # ... (existing Apple config) ...
    endif()
endif()

# Asset handling - skip copy for Android (handled by APK packaging)
if(NOT ANDROID AND NOT EXISTS "${CMAKE_BINARY_DIR}/config.yml")
    execute_process(COMMAND "${CMAKE_COMMAND}" -E copy_if_different 
        "${CMAKE_SOURCE_DIR}/config.yml" "${CMAKE_BINARY_DIR}/config.yml")
    execute_process(COMMAND "${CMAKE_COMMAND}" -E copy_directory 
        "${CMAKE_SOURCE_DIR}/assets/" "${CMAKE_BINARY_DIR}/assets/")
endif()

# Android: Final setup
if(ANDROID)
    # Define asset directory for Android
    target_compile_definitions(${PROJECT_NAME} PRIVATE 
        ASSET_DIR=\\\"assets/\\\"
        CONFIG_FILE=\\\"config.yml\\\"
    )
    
    # Install libraries for Android build
    install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
endif()
