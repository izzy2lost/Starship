cmake_minimum_required(VERSION 3.24.0)

option(NON_PORTABLE "Build a non-portable version" OFF)

project(libultraship LANGUAGES C CXX)

# ----- Platform-Specific Compiler Setup -----
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin" OR CMAKE_SYSTEM_NAME STREQUAL "iOS")
    enable_language(OBJCXX)
    set(CMAKE_OBJC_FLAGS "${CMAKE_OBJC_FLAGS} -fobjc-arc")
    set(CMAKE_OBJCXX_FLAGS "${CMAKE_OBJCXX_FLAGS} -fobjc-arc")
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

option(USE_OPENGLES "Enable GLES3" OFF)
option(GBI_UCODE "Specify the GBI ucode version" F3DEX_GBI_2)

# ========== SDL2 Include Directory Detection ==========
# The parent CMakeLists.txt should provide SDL2_INCLUDE_DIRS for all platforms!
# We'll try to use that. Fallback to sensible Android FetchContent location if defined.

set(_EXTRA_SDL2_INCLUDES "")
if (CMAKE_SYSTEM_NAME STREQUAL "Android")
    # If SDL2_INCLUDE_DIRS isn't set (sometimes it isn't with FetchContent), try the typical FetchContent dir.
    if(NOT SDL2_INCLUDE_DIRS)
        set(SDL2_INCLUDE_DIRS
            ${CMAKE_BINARY_DIR}/_deps/sdl2-src/include
        )
    endif()
    list(APPEND _EXTRA_SDL2_INCLUDES ${SDL2_INCLUDE_DIRS})
    list(APPEND _EXTRA_SDL2_INCLUDES ${SDL2_INCLUDE_DIRS}/SDL2)
else()
    if(SDL2_INCLUDE_DIRS)
        list(APPEND _EXTRA_SDL2_INCLUDES ${SDL2_INCLUDE_DIRS})
    endif()
endif()

# ========== Source Files ==========
file(GLOB_RECURSE LIBULTRASHIP_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_library(libultraship STATIC ${LIBULTRASHIP_SOURCES})
set_target_properties(libultraship PROPERTIES PREFIX "")

# ========== Platform-specific Links ==========
if (CMAKE_SYSTEM_NAME STREQUAL "Android")
    find_library(ANDROID_LOG_LIB log)
    if (ANDROID_LOG_LIB)
        target_link_libraries(libultraship PRIVATE ${ANDROID_LOG_LIB})
    endif()
endif()

if (USE_OPENGLES)
    if (CMAKE_SYSTEM_NAME STREQUAL "Android")
        find_library(OPENGLES_LIBRARY GLESv3)
        if(OPENGLES_LIBRARY)
            target_link_libraries(libultraship PRIVATE ${OPENGLES_LIBRARY})
        endif()
    else()
        find_library(OPENGLES_LIBRARY GLESv2)
        if(OPENGLES_LIBRARY)
            target_link_libraries(libultraship PRIVATE ${OPENGLES_LIBRARY})
        endif()
    endif()
endif()

# ========== Include Directories ==========
target_include_directories(libultraship
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${_EXTRA_SDL2_INCLUDES}
)
