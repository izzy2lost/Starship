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

# ======= Gather all .c and .cpp files in src/ (recursively) =======
file(GLOB_RECURSE LIBULTRASHIP_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# ======= Exclude unwanted directories or patterns if needed =======
foreach(file ${LIBULTRASHIP_SOURCES})
    if (
        file MATCHES "/third[_-]?party/"
        OR file MATCHES "/test/"
        OR file MATCHES "/build/"
        OR file MATCHES "/\\.git/"
        OR file MATCHES "/docs/"
        OR file MATCHES "~$"
    )
        list(REMOVE_ITEM LIBULTRASHIP_SOURCES "${file}")
    endif()
endforeach()

# ======= Build SHARED library for Android (libultraship.so) =======
add_library(libultraship SHARED ${LIBULTRASHIP_SOURCES})
set_target_properties(libultraship PROPERTIES PREFIX "")

# ======= Android logcat support =======
if (CMAKE_SYSTEM_NAME STREQUAL "Android")
    find_library(ANDROID_LOG_LIB log)
    if (ANDROID_LOG_LIB)
        target_link_libraries(libultraship PRIVATE ${ANDROID_LOG_LIB})
    endif()
endif()

# ======= GLES support (optional, for Android) =======
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

# ======= Include directories =======
# ----------- Include Directories -----------
target_include_directories(libultraship
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/libultraship/include/libultraship
        ${CMAKE_CURRENT_SOURCE_DIR}
)
