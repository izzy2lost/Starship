cmake_minimum_required(VERSION 3.16)

project(libultraship LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

option(USE_OPENGLES "Enable GLES3" OFF)

# macOS/ObjC++
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    enable_language(OBJCXX)
    set(CMAKE_OBJC_FLAGS "${CMAKE_OBJC_FLAGS} -fobjc-arc")
    set(CMAKE_OBJCXX_FLAGS "${CMAKE_OBJCXX_FLAGS} -fobjc-arc")
endif()

# MSVC runtime (static CRT)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Optionally: VCPKG auto-deps for Windows if desired
if (CMAKE_SYSTEM_NAME STREQUAL "Windows" AND USE_AUTO_VCPKG)
    include(cmake/automate-vcpkg.cmake)
    if ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        set(VCPKG_TRIPLET x86-windows-static)
        set(VCPKG_TARGET_TRIPLET x86-windows-static)
    elseif ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        set(VCPKG_TRIPLET x64-windows-static)
        set(VCPKG_TARGET_TRIPLET x64-windows-static)
    endif()
    vcpkg_bootstrap()
    vcpkg_install_packages(zlib bzip2 sdl2 glew)
endif()

add_subdirectory(extern)
add_subdirectory(src)

# --------- Platform-specific sources (for Android stub) ---------
set(LIBUTRASHIP_PLATFORM_SOURCES "")

if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    # Make sure this matches the location of your stub
    set(LIBUTRASHIP_PLATFORM_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/port/android/os_stub.c
    )
endif()

# ----------------- libultraship library target -----------------
# Collect ALL your libultraship sources here, for example:
set(LIBUTRASHIP_SOURCES
    # src/your_file1.cpp
    # src/your_file2.cpp
    # src/...
    # src/public/libultra/os.cpp
    # Add the rest of your sources here!
    ${LIBUTRASHIP_PLATFORM_SOURCES}
)

add_library(libultraship STATIC ${LIBUTRASHIP_SOURCES})
set_target_properties(libultraship PROPERTIES PREFIX "")

# --------------- INCLUDE DIRECTORIES -----------------
target_include_directories(libultraship
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/libultraship
        ${CMAKE_CURRENT_SOURCE_DIR}/src/public/libultra
        ${CMAKE_CURRENT_SOURCE_DIR}/extern/nlohmann-json/single_include
        ${CMAKE_CURRENT_SOURCE_DIR}/extern/nlohmann-json/include
        # add other include dirs as needed
        ${CMAKE_CURRENT_SOURCE_DIR}/extern/spdlog/include
        ${CMAKE_CURRENT_SOURCE_DIR}/extern/stb
        ${CMAKE_CURRENT_SOURCE_DIR}/src # (if your headers are here)
)
