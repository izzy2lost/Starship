cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)
project(Starship VERSION 2.0.0 LANGUAGES C CXX ASM)
include(FetchContent)

# --- Project Metadata ---
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

# --- Platform Specific ---
if(APPLE)
  enable_language(OBJCXX)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version")
endif()

if (WIN32)
  include(cmake/automate-vcpkg.cmake)
  set(VCPKG_TRIPLET x64-windows-static)
  set(VCPKG_TARGET_TRIPLET x64-windows-static)
  vcpkg_bootstrap()
  vcpkg_install_packages(zlib bzip2 libzip libpng sdl2 glew glfw3 nlohmann-json tinyxml2 spdlog libogg libvorbis)
  set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
  set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

if(IOS)
  set(PLATFORM "OS64")
  include(cmake/ios.toolchain.cmake)
  add_compile_definitions(PLATFORM_IOS=1)
  set(CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "YOUR_TEAM_ID")
  set(PROJECT_ID "dev.net64.game")
endif()

if(NOT EXISTS ${CMAKE_BINARY_DIR}/config.yml)
  execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/config.yml" "${CMAKE_BINARY_DIR}/config.yml")
  execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/assets/" "${CMAKE_BINARY_DIR}/assets/")
endif()

# --- Standards ---
set(CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard to use")
set(CMAKE_C_STANDARD 11 CACHE STRING "The C standard to use")

# --- Flags and Compile Definitions ---
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

# --- Global Project Definitions ---
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
  YAML_CPP_STATIC_DEFINE
)

# --- OpenGL (Linux Only) ---
if (UNIX AND NOT APPLE AND NOT ANDROID)
  find_package(OpenGL REQUIRED)
endif()

# --- configure_file ---
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/properties.h.in ${CMAKE_CURRENT_SOURCE_DIR}/properties.h @ONLY)

# --- Collect Source Files ---
file(GLOB_RECURSE ALL_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  "src/audio/*.c" "src/audio/*.h"
  "src/engine/*.c" "src/engine/*.h"
  "src/libultra/io/aisetfreq.c"
  "src/libultra/gu/sqrtf.c"
  "src/libultra/gu/mtxutil.c"
  "src/libultra/gu/ortho.c"
  "src/overlays/*.c" "src/overlays/*.h"
  "src/port/*.h" "src/port/*.c" "src/port/*.cpp"
  "src/sys/*.c" "src/sys/*.h"
  "src/1FA60.c" "src/buffers.c" "src/dmatable.c"
  "src/libc_math64.c" "src/libc_sprintf.c"
)

# --- Exclude Specific Files ---
list(FILTER ALL_FILES EXCLUDE REGEX ".*\\.inc\\.c")
list(FILTER ALL_FILES EXCLUDE REGEX "src/engine/fox_colheaders.c")
list(FILTER ALL_FILES EXCLUDE REGEX "src/engine/fox_edata_info.c")
list(FILTER ALL_FILES EXCLUDE REGEX "src/engine/fox_rcp_setup.c")
list(FILTER ALL_FILES EXCLUDE REGEX "src/engine/fox_load_inits.c")
list(FILTER ALL_FILES EXCLUDE REGEX "src/overlays/ovl_ending/fox_end2_data.c")
list(FILTER ALL_FILES EXCLUDE REGEX "src/sys/sys_timer.c")
list(FILTER ALL_FILES EXCLUDE REGEX "src/sys/sys_fault.c")
list(FILTER ALL_FILES EXCLUDE REGEX "src/mods/object_ram.c")

# --- Android: Exclude Vorbis ---
if (CMAKE_SYSTEM_NAME STREQUAL "Android")
  list(FILTER ALL_FILES EXCLUDE REGEX "vorbis")
endif()

# --- Target Type ---
if(CMAKE_SYSTEM_NAME STREQUAL "Android")
  add_library(${PROJECT_NAME} SHARED ${ALL_FILES})
else()
  add_executable(${PROJECT_NAME} ${ALL_FILES})
endif()

# --- SSE2NEON (Header Only) ---
set(SSE2NEON_DIR ${CMAKE_BINARY_DIR}/_deps/sse2neon)
file(DOWNLOAD "https://raw.githubusercontent.com/DLTcollab/sse2neon/refs/heads/master/sse2neon.h" "${SSE2NEON_DIR}/sse2neon.h")
target_include_directories(${PROJECT_NAME} PRIVATE ${SSE2NEON_DIR})

# --- dr_libs ---
FetchContent_Declare(
  dr_libs
  GIT_REPOSITORY https://github.com/mackron/dr_libs.git
  GIT_TAG da35f9d6c7374a95353fd1df1d394d44ab66cf01
)
FetchContent_MakeAvailable(dr_libs)

#==============================================================================#
# Libultraship Integration                                                     #
#==============================================================================#
add_subdirectory(libultraship)
add_dependencies(${PROJECT_NAME} libultraship)
target_link_libraries(${PROJECT_NAME} PRIVATE libultraship)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
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
  ${dr_libs_SOURCE_DIR}
)

# --- Platform-specific Linkage ---
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

elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:_DEBUG;_CRT_SECURE_NO_WARNINGS;ENABLE_DX11;>"
    "$<$<CONFIG:Release>:NDEBUG>"
    "INCLUDE_GAME_PRINTF;UNICODE;_UNICODE;_CRT_SECURE_NO_WARNINGS;_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS;"
  )
  add_compile_definitions(STORMLIB_NO_AUTO_LINK)
  set(STORMLIB_NO_AUTO_LINK ON)
  find_package(Ogg CONFIG REQUIRED)
  find_package(Vorbis CONFIG REQUIRED)
  target_link_libraries(${PROJECT_NAME} PRIVATE
    Ogg::ogg
    Vorbis::vorbis
    Vorbis::vorbisenc
    Vorbis::vorbisfile
  )

elseif(CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
  find_package(SDL2)
  find_package(Vorbis REQUIRED)
  find_package(Ogg REQUIRED)
  target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL2::SDL2
    Vorbis::vorbisfile
    Ogg::ogg
    -lglad
  )

elseif(CMAKE_SYSTEM_NAME STREQUAL "CafeOS")
  set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "$<$<CONFIG:Debug>:-Wl,--wrap=abort>"
  )
  target_include_directories(${PROJECT_NAME} PRIVATE ${DEVKITPRO}/portlibs/wiiu/include/)

else() # Linux, Mac, etc.
  find_package(Ogg REQUIRED)
  find_package(Vorbis REQUIRED)
  target_link_libraries(${PROJECT_NAME} PRIVATE
    Ogg::ogg
    Vorbis::vorbis
    Vorbis::vorbisenc
    Vorbis::vorbisfile
  )
endif()

# --- Networking (optional) ---
if(USE_NETWORKING)
  if(MSVC)
    vcpkg_install_packages(sdl2-net)
  endif()
  find_package(SDL2_net REQUIRED)
  target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_net::SDL2_net)
  target_compile_definitions(${PROJECT_NAME} PRIVATE USE_NETWORKING)
endif()

option(USE_STANDALONE "Build as a standalone executable" OFF)
option(BUILD_STORMLIB "Build with StormLib support" OFF)
option(BUILD_SM64 "Build with Super Mario 64 support" OFF)
option(BUILD_MK64 "Build with Mario Kart 64 support" OFF)
option(BUILD_FZERO "Build with F-Zero X support" OFF)

add_subdirectory(tools/Torch)
target_link_libraries(${PROJECT_NAME} PRIVATE torch)

# ====================== Compiler & Linker Options ===========================

if (MSVC)
    # Detect target arch
    if ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:/w;/Od;/MTd;/ZI>
            $<$<CONFIG:Release>:/Oi;/Gy;/W3;/MT;/Zi>
            /permissive-;/MP;${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:/INCREMENTAL>
            $<$<CONFIG:Release>:/OPT:REF;/OPT:ICF;/INCREMENTAL:NO;/FORCE:MULTIPLE>
            /MANIFEST:NO;/DEBUG;/SUBSYSTEM:WINDOWS
        )
    elseif ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:/MTd>
            $<$<CONFIG:Release>:/O2;/Oi;/Gy;/MT>
            /permissive-;/MP;/w;${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:/STACK:8777216>
            $<$<CONFIG:Release>:/OPT:REF;/OPT:ICF;/INCREMENTAL:NO;/FORCE:MULTIPLE>
            /MANIFEST:NO;/DEBUG;/SUBSYSTEM:WINDOWS
        )
    endif()
    # Remove /RTC from MSVC flags globally
    foreach(fentry CMAKE_C_FLAGS CMAKE_CXX_FLAGS)
        string(REGEX REPLACE "/RTC(su|[1su])" "" ${fentry} "${${fentry}}")
    endforeach()
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    if (CMAKE_SYSTEM_NAME STREQUAL "Darwin" OR CMAKE_SYSTEM_NAME STREQUAL "iOS")
        target_compile_options(${PROJECT_NAME} PRIVATE
            -Wall -Wextra -Wno-error
            -Wno-return-type -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable
            -Wno-missing-field-initializers -Wno-parentheses -Wno-narrowing -Wno-missing-braces
            -Wno-int-conversion
            $<$<COMPILE_LANGUAGE:C>:-Werror-implicit-function-declaration;-Wno-incompatible-pointer-types;-fpermissive>
            $<$<COMPILE_LANGUAGE:CXX>:-fpermissive;-Wno-c++11-narrowing;-Wno-deprecated-enum-enum-conversion>
            -pthread
        )
        target_link_options(${PROJECT_NAME} PRIVATE -pthread)
    elseif (CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
        target_compile_options(${PROJECT_NAME} PRIVATE
            -Wall -Wextra -Wno-error
            -Wno-return-type -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable
            -Wno-missing-field-initializers -Wno-parentheses -Wno-narrowing -Wno-missing-braces
            -Wno-int-conversion
            $<$<COMPILE_LANGUAGE:C>:-Werror-implicit-function-declaration;-Wno-incompatible-pointer-types;-fpermissive>
            $<$<COMPILE_LANGUAGE:CXX>:-fpermissive;-Wno-c++11-narrowing;-Wno-deprecated-enum-enum-conversion>
            -pthread
        )
        target_link_options(${PROJECT_NAME} PRIVATE -pthread)
    elseif (CMAKE_SYSTEM_NAME STREQUAL "CafeOS")
        target_compile_options(${PROJECT_NAME} PRIVATE
            -O2
            # disable some warnings to not clutter output
            -Wno-multichar -Wno-return-type -Wno-narrowing -Wno-switch-outside-range
            $<$<COMPILE_LANGUAGE:C>:-Werror-implicit-function-declaration;-Wno-incompatible-pointer-types;-Wno-discarded-array-qualifiers;-Wno-discarded-qualifiers;-Wno-int-conversion;-Wno-builtin-declaration-mismatch;-Wno-switch-unreachable;-Wno-stringop-overflow>
        )
    else() # Linux, cross-compile, etc.
        set(CPU_OPTION "")
        if(NOT ANDROID)
            if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
                set(CPU_OPTION -msse2 -mfpmath=sse)
            elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
                set(CPU_OPTION -mcpu=native)
            endif()
        endif()
        target_compile_options(${PROJECT_NAME} PRIVATE
            -Wall -Wextra -Wno-error
            -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable
            -Wno-missing-field-initializers -Wno-parentheses -Wno-narrowing -Wno-missing-braces
            -Wno-int-conversion
            $<$<COMPILE_LANGUAGE:C>:-Werror-implicit-function-declaration;-Wno-incompatible-pointer-types;-fpermissive>
            $<$<COMPILE_LANGUAGE:CXX>:-fpermissive;-Wno-deprecated-enum-enum-conversion>
            -pthread
            ${CPU_OPTION}
        )
        target_link_options(${PROJECT_NAME} PRIVATE -pthread -Wl,-export-dynamic)
    endif()
endif()
