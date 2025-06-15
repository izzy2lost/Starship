cmake_minimum_required(VERSION 3.24.0)

option(NON_PORTABLE "Build a non-portable version" OFF)

project(libultraship LANGUAGES C CXX)

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

# --- SDL2 include dir for Android ---
if (CMAKE_SYSTEM_NAME STREQUAL "Android" AND NOT SDL2_INCLUDE_DIR)
    if(EXISTS "${CMAKE_BINARY_DIR}/_deps/sdl2-src/include")
        set(SDL2_INCLUDE_DIR "${CMAKE_BINARY_DIR}/_deps/sdl2-src/include")
    elseif(EXISTS "${SDL2_SOURCE_DIR}/include")
        set(SDL2_INCLUDE_DIR "${SDL2_SOURCE_DIR}/include")
    endif()
endif()

file(GLOB_RECURSE LIBULTRASHIP_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_library(libultraship STATIC ${LIBULTRASHIP_SOURCES})
set_target_properties(libultraship PROPERTIES PREFIX "")

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

target_include_directories(libultraship
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        $<$<STREQUAL:${CMAKE_SYSTEM_NAME},Android>:${SDL2_INCLUDE_DIR}>
)
