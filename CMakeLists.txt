cmake_minimum_required(VERSION 3.24.0)

# ==================== Project Info ====================
project(Starship VERSION 2.0.0 LANGUAGES C CXX ASM)
include(FetchContent)

set(NATO_PHONETIC_ALPHABET
  "Alfa" "Bravo" "Charlie" "Delta" "Echo" "Foxtrot" "Golf" "Hotel"
  "India" "Juliett" "Kilo" "Lima" "Mike" "November" "Oscar" "Papa"
  "Quebec" "Romeo" "Sierra" "Tango" "Uniform" "Victor" "Whiskey"
  "Xray" "Yankee" "Zulu"
)
math(EXPR PATCH_INDEX "${PROJECT_VERSION_PATCH}")
list(GET NATO_PHONETIC_ALPHABET ${PATCH_INDEX} PROJECT_PATCH_WORD)
set(PROJECT_BUILD_NAME "Barnard ${PROJECT_PATCH_WORD}" CACHE STRING "" FORCE)
set(PROJECT_TEAM "SonicDcer & Lywx" CACHE STRING "" FORCE)

# ==================== Platform-specific ====================
if(ANDROID)
    set(ANDROID_STL c++_shared)
    set(ANDROID_PLATFORM android-24)
    add_definitions(-DANDROID -DPLATFORM_ANDROID=1)
endif()

if(APPLE)
    enable_language(OBJCXX)
endif()

set(CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard to use")
set(CMAKE_C_STANDARD 11 CACHE STRING "The C standard to use")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")
set(YAML_CPP_STATIC_DEFINE ON)
add_compile_definitions(YAML_CPP_STATIC_DEFINE)

# VCPKG (Windows only)
if (WIN32)
    include(cmake/automate-vcpkg.cmake)
    set(VCPKG_TRIPLET x64-windows-static)
    set(VCPKG_TARGET_TRIPLET x64-windows-static)
    vcpkg_bootstrap()
    vcpkg_install_packages(zlib bzip2 libzip libpng sdl2 glew glfw3 nlohmann-json tinyxml2 spdlog libogg libvorbis)
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

# Copy config/assets on first configure
if(NOT EXISTS ${CMAKE_BINARY_DIR}/config.yml)
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/config.yml" "${CMAKE_BINARY_DIR}/config.yml")
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/assets/" "${CMAKE_BINARY_DIR}/assets/")
endif()

if(MSVC)
    set(CPP "${CMAKE_C_COMPILER}" "/EP")
else()
    set(CPP "${CMAKE_C_COMPILER}" "-E" "-P" "-Wno-trigraphs" "-x" "c")
endif()

if(IOS)
    set(PLATFORM "OS64")
    include(cmake/ios.toolchain.cmake)
    add_compile_definitions(PLATFORM_IOS=1)
    set(CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "YOUR_TEAM_ID")
    set(PROJECT_ID "dev.net64.game")
endif()

# ==================== Compiler/Release Flags ====================
if (CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
    set(CMAKE_C_FLAGS_DEBUG "-g -ffast-math -DDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -ffast-math -DDEBUG")
    set(CMAKE_C_FLAGS_RELEASE "-O3 -ffast-math -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ffast-math -DNDEBUG")
else()
    set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
    set(CMAKE_OBJCXX_FLAGS_RELEASE "-O2 -DNDEBUG")
endif()

set(VERSION us)
set(USE_NETWORKING OFF)
set(SKIP_XCODE_VERSION_CHECK ON)

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
)

if (UNIX AND NOT APPLE AND NOT ANDROID)
    find_package(OpenGL REQUIRED)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/properties.h.in ${CMAKE_CURRENT_SOURCE_DIR}/properties.h @ONLY)

# ==================== Source Files ====================
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

file(GLOB_RECURSE ALL_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "src/audio/*.c"
    "src/audio/*.h"
    "src/engine/*.c"
    "src/engine/*.h"
    "src/libultra/io/aisetfreq.c"
    "src/libultra/gu/sqrtf.c"
    "src/libultra/gu/mtxutil.c"
    "src/libultra/gu/ortho.c"
    "src/overlays/*.c"
    "src/overlays/*.h"
    "src/port/*.h"
    "src/port/*.c"
    "src/port/*.cpp"
    "src/sys/*.c"
    "src/sys/*.h"
    "src/1FA60.c"
    "src/buffers.c"
    "src/dmatable.c"
    "src/libc_math64.c"
    "src/libc_sprintf.c"
    # "Resource.rc" # Commented out for Android; needed for Windows resource
)

# Exclude specific files from the ALL_FILES list
list(FILTER ALL_FILES EXCLUDE REGEX ".*.inc.c")
list(FILTER ALL_FILES EXCLUDE REGEX "src/engine/fox_colheaders.c")
list(FILTER ALL_FILES EXCLUDE REGEX "src/engine/fox_edata_info.c")
list(FILTER ALL_FILES EXCLUDE REGEX "src/engine/fox_rcp_setup.c")
list(FILTER ALL_FILES EXCLUDE REGEX "src/engine/fox_load_inits.c")
list(FILTER ALL_FILES EXCLUDE REGEX "src/overlays/ovl_ending/fox_end2_data.c")
list(FILTER ALL_FILES EXCLUDE REGEX "src/sys/sys_timer.c")
list(FILTER ALL_FILES EXCLUDE REGEX "src/sys/sys_fault.c")
list(FILTER ALL_FILES EXCLUDE REGEX "src/mods/object_ram.c")

# ==================== SDL2 Find/FETCH ====================
if (CMAKE_SYSTEM_NAME STREQUAL "Android")
    find_package(SDL2 QUIET)
    if (NOT SDL2_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            SDL2
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG release-2.32.0
        )
        message("SDL2 not found. Downloading and building SDL2 for Android...")
        FetchContent_MakeAvailable(SDL2)
    endif()
elseif (WIN32)
    find_package(SDL2 CONFIG REQUIRED)
else()
    find_package(SDL2 REQUIRED)
endif()

# ==================== Build Target ====================
if(ANDROID)
    add_library(${PROJECT_NAME} SHARED ${ALL_FILES})
else()
    add_executable(${PROJECT_NAME} ${ALL_FILES})
endif()

# ==================== SSE2NEON + dr_libs ====================
set(SSE2NEON_DIR ${CMAKE_BINARY_DIR}/_deps/sse2neon)
file(DOWNLOAD "https://raw.githubusercontent.com/DLTcollab/sse2neon/refs/heads/master/sse2neon.h" "${SSE2NEON_DIR}/sse2neon.h")
include_directories(${SSE2NEON_DIR})

FetchContent_Declare(
    dr_libs
    GIT_REPOSITORY https://github.com/mackron/dr_libs.git
    GIT_TAG da35f9d6c7374a95353fd1df1d394d44ab66cf01
)
FetchContent_MakeAvailable(dr_libs)

# ==================== Libultraship Integration ====================
add_subdirectory(libultraship)

target_include_directories(${PROJECT_NAME} PRIVATE
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
    ${dr_libs_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE libultraship)

# ===== Link SDL2 as a target =====
if (CMAKE_SYSTEM_NAME STREQUAL "Android")
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2)
elseif (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2::SDL2main)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)
endif()

# ==================== Platform-specific links/defs ====================
if(ANDROID)
    find_library(LOG_LIB log)
    if(LOG_LIB)
        target_link_libraries(${PROJECT_NAME} PRIVATE ${LOG_LIB})
    endif()
    option(USE_OPENGLES "Enable GLES3" ON)
    if(USE_OPENGLES)
        find_library(GLES_LIB GLESv3)
        if(GLES_LIB)
            target_link_libraries(${PROJECT_NAME} PRIVATE ${GLES_LIB})
        endif()
    endif()
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:_DEBUG;_CRT_SECURE_NO_WARNINGS;ENABLE_DX11;>"
        "$<$<CONFIG:Release>:NDEBUG>"
        "INCLUDE_GAME_PRINTF;UNICODE;_UNICODE;_CRT_SECURE_NO_WARNINGS;_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS;"
    )
    add_compile_definitions(STORMLIB_NO_AUTO_LINK)
    set(STORMLIB_NO_AUTO_LINK ON)
elseif (CMAKE_SYSTEM_NAME STREQUAL "CafeOS")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:_DEBUG>"
        "$<$<CONFIG:Release>:NDEBUG>"
        "SPDLOG_ACTIVE_LEVEL=3;SPDLOG_NO_THREAD_ID;SPDLOG_NO_TLS;STBI_NO_THREAD_LOCALS;"
    )
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU|Clang|AppleClang")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:_DEBUG>"
        "$<$<CONFIG:Release>:NDEBUG>"
        "$<$<BOOL:${BUILD_CROWD_CONTROL}>:ENABLE_CROWD_CONTROL>"
        "SPDLOG_ACTIVE_LEVEL=0;_CONSOLE;_CRT_SECURE_NO_WARNINGS;UNICODE;_UNICODE"
    )
endif()

################################################################################
# Platform-specific Packaging and Extras
################################################################################

if(CMAKE_SYSTEM_NAME MATCHES "NintendoSwitch")
    nx_generate_nacp(${PROJECT_NAME}.nacp
        NAME "${PROJECT_NAME}"
        AUTHOR "${PROJECT_TEAM}"
        VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}"
    )
    nx_create_nro(${PROJECT_NAME}
        NACP ${PROJECT_NAME}.nacp
        ICON ${CMAKE_CURRENT_SOURCE_DIR}/nx-logo.jpg
    )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.nro DESTINATION . COMPONENT ${PROJECT_NAME})
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    add_custom_target(CreateOSXIcons
        COMMAND mkdir -p ${CMAKE_BINARY_DIR}/macosx/starship.iconset
        COMMAND sips -z 16 16     logo.png --out ${CMAKE_BINARY_DIR}/macosx/starship.iconset/icon_16x16.png
        COMMAND sips -z 32 32     logo.png --out ${CMAKE_BINARY_DIR}/macosx/starship.iconset/icon_16x16@2x.png
        COMMAND sips -z 32 32     logo.png --out ${CMAKE_BINARY_DIR}/macosx/starship.iconset/icon_32x32.png
        COMMAND sips -z 64 64     logo.png --out ${CMAKE_BINARY_DIR}/macosx/starship.iconset/icon_32x32@2x.png
        COMMAND sips -z 128 128   logo.png --out ${CMAKE_BINARY_DIR}/macosx/starship.iconset/icon_128x128.png
        COMMAND sips -z 256 256   logo.png --out ${CMAKE_BINARY_DIR}/macosx/starship.iconset/icon_128x128@2x.png
        COMMAND sips -z 256 256   logo.png --out ${CMAKE_BINARY_DIR}/macosx/starship.iconset/icon_256x256.png
        COMMAND sips -z 512 512   logo.png --out ${CMAKE_BINARY_DIR}/macosx/starship.iconset/icon_256x256@2x.png
        COMMAND sips -z 512 512   logo.png --out ${CMAKE_BINARY_DIR}/macosx/starship.iconset/icon_512x512.png
        COMMAND cp logo.png ${CMAKE_BINARY_DIR}/macosx/starship.iconset/icon_512x512@2x.png
        COMMAND iconutil -c icns -o ${CMAKE_BINARY_DIR}/macosx/starship.icns ${CMAKE_BINARY_DIR}/macosx/starship.iconset
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Creating OSX icons ..."
    )
    add_dependencies(${PROJECT_NAME} CreateOSXIcons)
    configure_file("${CMAKE_SOURCE_DIR}/Info.plist" "${CMAKE_BINARY_DIR}/Info.plist" COPYONLY)
endif()

################################################################################
# Packaging system
################################################################################

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(CPACK_GENERATOR "External")
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows|NintendoSwitch|CafeOS")
    set(CPACK_GENERATOR "ZIP")
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(CPACK_GENERATOR "Bundle")
endif()

set(CPACK_PROJECT_CONFIG_FILE ${CMAKE_SOURCE_DIR}/cmake/configure-packaging.cmake)
include(cmake/packaging.cmake)
