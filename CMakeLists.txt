cmake_minimum_required(VERSION 3.24.0)

project(libultraship LANGUAGES C CXX)

option(USE_OPENGLES "Enable GLES3" OFF)
option(NON_PORTABLE "Build a non-portable version" OFF)

# ==== Platform-specific Options ====
if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    option(SIGN_LIBRARY "Enable xcode signing" OFF)
    option(BUNDLE_ID "Bundle ID for xcode signing" "com.example.libultraship")
    set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin" OR CMAKE_SYSTEM_NAME STREQUAL "iOS")
    enable_language(OBJCXX)
    set(CMAKE_OBJC_FLAGS "${CMAKE_OBJC_FLAGS} -fobjc-arc")
    set(CMAKE_OBJCXX_FLAGS "${CMAKE_OBJCXX_FLAGS} -fobjc-arc")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# ==== Source Collection (Smart, Exclude Unwanted Dirs) ====
file(GLOB_RECURSE LIBULTRASHIP_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

foreach(file ${LIBULTRASHIP_SOURCES})
    if(file MATCHES "/third[_-]?party/"
       OR file MATCHES "/test/"
       OR file MATCHES "/build/"
       OR file MATCHES "/\\.git/"
       OR file MATCHES "/docs/"
       OR file MATCHES "~$")
        list(REMOVE_ITEM LIBULTRASHIP_SOURCES "${file}")
    endif()
endforeach()

add_library(libultraship STATIC ${LIBULTRASHIP_SOURCES})
set_target_properties(libultraship PROPERTIES PREFIX "")

# ==== Android: logcat and GLES Support ====
if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    find_library(ANDROID_LOG_LIB log)
    if(ANDROID_LOG_LIB)
        target_link_libraries(libultraship PRIVATE ${ANDROID_LOG_LIB})
    endif()

    if(USE_OPENGLES)
        find_library(OPENGLES_LIBRARY GLESv3)
        if(OPENGLES_LIBRARY)
            target_link_libraries(libultraship PRIVATE ${OPENGLES_LIBRARY})
        endif()
    endif()
elseif(USE_OPENGLES)
    find_library(OPENGLES_LIBRARY GLESv2)
    if(OPENGLES_LIBRARY)
        target_link_libraries(libultraship PRIVATE ${OPENGLES_LIBRARY})
    endif()
endif()

# ==== Include Directories ====
target_include_directories(libultraship
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_BINARY_DIR}
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
        ${SDL2_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS}
        ${dr_libs_SOURCE_DIR}
        # add more include dirs here if needed
)
