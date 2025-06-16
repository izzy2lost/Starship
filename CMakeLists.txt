cmake_minimum_required(VERSION 3.16)
project(Starship LANGUAGES C CXX ASM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# --- Platform Defines ---
if(ANDROID)
    add_definitions(-DANDROID -DPLATFORM_ANDROID=1)
endif()

# --- Compile Definitions (adjust as needed) ---
add_compile_definitions(
    ENABLE_RUMBLE=1
    F3DEX_GBI=1
    NON_MATCHING=1
    NON_EQUIVALENT=1
    VERSION_US=1
    _LANGUAGE_C
    GBI_FLOATS=1
)

# --- Source Files ---
file(GLOB_RECURSE ALL_SRC
    "src/*.c" "src/*.cpp" "src/*.h"
)
list(FILTER ALL_SRC EXCLUDE REGEX ".*\\.inc\\.c")
# Add/remove other filters as needed for your project

# --- SDL2 for Android: FetchContent ---
if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    find_package(SDL2 QUIET)
    if(NOT SDL2_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            SDL2
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG release-2.32.0
            CMAKE_ARGS -DSDL_FILE_SUPPORTS_LARGE_FILES=OFF
        )
        message(STATUS "Fetching SDL2 (Android, large file support OFF)")
        FetchContent_MakeAvailable(SDL2)
    endif()
endif()

# --- Build Target ---
if(ANDROID)
    add_library(${PROJECT_NAME} SHARED ${ALL_SRC})
else()
    add_executable(${PROJECT_NAME} ${ALL_SRC})
endif()

# --- Example: Add libultraship subdir ---
add_subdirectory(libultraship)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libultraship/include)
target_link_libraries(${PROJECT_NAME} PRIVATE libultraship)

# --- Platform Linkage ---
if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2)
    find_library(LOG_LIB log)
    if(LOG_LIB)
        target_link_libraries(${PROJECT_NAME} PRIVATE ${LOG_LIB})
    endif()
    find_library(GLES_LIB GLESv3)
    if(GLES_LIB)
        target_link_libraries(${PROJECT_NAME} PRIVATE ${GLES_LIB})
    endif()
elseif(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main)
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)
endif()

# --- Optional: dr_libs like 2soh does ---
include(FetchContent)
FetchContent_Declare(
    dr_libs
    GIT_REPOSITORY https://github.com/mackron/dr_libs.git
    GIT_TAG da35f9d6c7374a95353fd1df1d394d44ab66cf01
)
FetchContent_MakeAvailable(dr_libs)
target_include_directories(${PROJECT_NAME} PRIVATE ${dr_libs_SOURCE_DIR})

# --- Packaging etc. as needed (optional) ---
