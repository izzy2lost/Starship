cmake_minimum_required(VERSION 3.24.0)

project(libultraship LANGUAGES C CXX)

option(USE_OPENGLES "Enable GLES3" OFF)
option(NON_PORTABLE "Build a non-portable version" OFF)

if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    option(SIGN_LIBRARY "Enable xcode signing" OFF)
    option(BUNDLE_ID "Bundle ID for xcode signing" "com.example.libultraship")
    set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin" OR CMAKE_SYSTEM_NAME STREQUAL "iOS")
    enable_language(OBJCXX)
    set(CMAKE_OBJC_FLAGS "${CMAKE_OBJC_FLAGS} -fobjc-arc")
    set(CMAKE_OBJCXX_FLAGS "${CMAKE_OBJCXX_FLAGS} -fobjc-arc")
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# =========== Sources =============

file(GLOB_RECURSE LIBULTRASHIP_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_library(libultraship STATIC ${LIBULTRASHIP_SOURCES})
set_target_properties(libultraship PROPERTIES PREFIX "")

# ----------- Android: logcat support -----------
if (CMAKE_SYSTEM_NAME STREQUAL "Android")
    find_library(ANDROID_LOG_LIB log)
    if (ANDROID_LOG_LIB)
        target_link_libraries(libultraship PRIVATE ${ANDROID_LOG_LIB})
    endif()
endif()

# ----------- GLES3 Support (optional, for Android) -----------
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

# ----------- Include Directories (add more as needed) -----------
target_include_directories(libultraship
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        # add more include dirs here if needed, like "include", "extern", etc.
)
