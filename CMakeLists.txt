cmake_minimum_required(VERSION 3.16...3.31 FATAL_ERROR)

# Main Project Info
project(Starship VERSION 2.0.0 LANGUAGES C CXX ASM)
include(FetchContent)

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

# Apple language tweaks
if(APPLE)
  enable_language(OBJCXX)
endif()

# Set platform defaults
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

# CMake Modules/Defines
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/cmake/modules/")
set(YAML_CPP_STATIC_DEFINE ON)
add_compile_definitions(YAML_CPP_STATIC_DEFINE)

# Windows-specific vcpkg setup
if(WIN32)
  include(cmake/automate-vcpkg.cmake)
  set(VCPKG_TRIPLET x64-windows-static)
  set(VCPKG_TARGET_TRIPLET x64-windows-static)
  vcpkg_bootstrap()
  vcpkg_install_packages(zlib bzip2 libzip libpng sdl2 glew glfw3 nlohmann-json tinyxml2 spdlog libogg libvorbis)
  set_property(DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT "${PROJECT_NAME}")
  set_property(DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif()

# Ensure config/assets present
if(NOT EXISTS "${CMAKE_BINARY_DIR}/config.yml")
  execute_process(COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_SOURCE_DIR}/config.yml" "${CMAKE_BINARY_DIR}/config.yml")
  execute_process(COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/assets/" "${CMAKE_BINARY_DIR}/assets/")
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
)

# Apple/iOS tweaks
if(IOS)
  set(PLATFORM "OS64")
  include(cmake/ios.toolchain.cmake)
  add_compile_definitions(PLATFORM_IOS=1)
  set(CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "YOUR_TEAM_ID")
  set(PROJECT_ID "dev.net64.game")
endif()

# Visual Studio arch safety
if(WIN32 AND CMAKE_SYSTEM_NAME STREQUAL "Windows")
  if(NOT CMAKE_VS_PLATFORM_NAME)
    set(CMAKE_VS_PLATFORM_NAME "x64")
  endif()
  message("${CMAKE_VS_PLATFORM_NAME} architecture in use")
  if(NOT (CMAKE_VS_PLATFORM_NAME STREQUAL "x64" OR CMAKE_VS_PLATFORM_NAME STREQUAL "Win32"))
    message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
  endif()
endif()

# Set compile flags for release/debug
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

# Build version, networking
set(VERSION us)
set(USE_NETWORKING OFF)
set(SKIP_XCODE_VERSION_CHECK ON)

# OpenGL is *not* needed on Android
if (UNIX AND NOT APPLE AND NOT (CMAKE_SYSTEM_NAME STREQUAL "Android"))
  find_package(OpenGL REQUIRED)
endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/properties.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/properties.h" @ONLY)

# Project include directories
include_directories(
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# Gather source files
file(GLOB_RECURSE ALL_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  "src/audio/*.c" "src/audio/*.h"
  "src/engine/*.c" "src/engine/*.h"
  "src/libultra/io/aisetfreq.c"
  "src/libultra/gu/sqrtf.c"
  "src/libultra/gu/mtxutil.c"
  "src/libultra/gu/ortho.c"
  "src/overlays/*.c" "src/overlays/*.h"
  "src/port/*.h" "src/port/*.c" "src/port/*.cpp"
  "src/sys/*.c" "src/sys/*.h"
  "src/1FA60.c"
  "src/buffers.c"
  "src/dmatable.c"
  "src/libc_math64.c"
  "src/libc_sprintf.c"
  "Resource.rc"
)
# Exclude specific files
list(FILTER ALL_FILES EXCLUDE REGEX ".*.inc.c")
list(FILTER ALL_FILES EXCLUDE REGEX "src/engine/fox_colheaders.c")
list(FILTER ALL_FILES EXCLUDE REGEX "src/engine/fox_edata_info.c")
list(FILTER ALL_FILES EXCLUDE REGEX "src/engine/fox_rcp_setup.c")
list(FILTER ALL_FILES EXCLUDE REGEX "src/engine/fox_load_inits.c")
list(FILTER ALL_FILES EXCLUDE REGEX "src/overlays/ovl_ending/fox_end2_data.c")
list(FILTER ALL_FILES EXCLUDE REGEX "src/sys/sys_timer.c")
list(FILTER ALL_FILES EXCLUDE REGEX "src/sys/sys_fault.c")
list(FILTER ALL_FILES EXCLUDE REGEX "src/mods/object_ram.c")

# iOS bundle target, else regular target
if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
  set(IOS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libultraship/ios")
  set(STORYBOARD_FILE "${IOS_DIR}/Launch.storyboard")
  set(IMAGE_FILES "${IOS_DIR}/PoweredBy.png")
  set(ICON_FILES "${IOS_DIR}/Icon.png")
  list(APPEND ALL_FILES ${STORYBOARD_FILE} ${IMAGE_FILES} ${ICON_FILES})

  add_executable(${PROJECT_NAME} ${ALL_FILES})
  set_xcode_property(${PROJECT_NAME} PRODUCT_BUNDLE_IDENTIFIER ${PROJECT_ID} All)
  set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST "${IOS_DIR}/plist.in"
    RESOURCE "${IMAGE_FILES};${STORYBOARD_FILE};${ICON_FILES}"
  )
else()
  add_executable(${PROJECT_NAME} ${ALL_FILES})
endif()

################################################################################
# MSVC runtime library (only for Windows/MSVC)
################################################################################
if(MSVC AND CMAKE_SYSTEM_NAME STREQUAL "Windows")
  get_property(MSVC_RUNTIME_LIBRARY_DEFAULT TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY)
  if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64" OR "${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    string(CONCAT MSVC_RUNTIME_LIBRARY_STR
      $<$<CONFIG:Debug>:MultiThreadedDebug>
      $<$<CONFIG:Release>:MultiThreaded>
      $<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
    set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})
  endif()
endif()

#=================== SSE2NEON ===================
set(SSE2NEON_DIR "${CMAKE_BINARY_DIR}/_deps/sse2neon")
file(DOWNLOAD "https://raw.githubusercontent.com/DLTcollab/sse2neon/refs/heads/master/sse2neon.h" "${SSE2NEON_DIR}/sse2neon.h")
include_directories("${SSE2NEON_DIR}")

#=================== dr_libs ====================
FetchContent_Declare(
    dr_libs
    GIT_REPOSITORY https://github.com/mackron/dr_libs.git
    GIT_TAG da35f9d6c7374a95353fd1df1d394d44ab66cf01
)
FetchContent_MakeAvailable(dr_libs)

#==============================================================================#
# Libultraship Integration                                                     #
#==============================================================================#
if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    set(LIBUTRASHIP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Android/app/jni/src/libultraship")
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
    ${SDL2_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${dr_libs_SOURCE_DIR}
)

add_subdirectory("${LIBUTRASHIP_DIR}" "${CMAKE_BINARY_DIR}/libultraship")
add_dependencies(${PROJECT_NAME} libultraship)
target_link_libraries(${PROJECT_NAME} PRIVATE libultraship)

# ---- Per-platform Defines and Link Dependencies ----
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:_DEBUG;_CRT_SECURE_NO_WARNINGS;ENABLE_DX11;>"
        "$<$<CONFIG:Release>:NDEBUG>"
        "INCLUDE_GAME_PRINTF;UNICODE;_UNICODE;_CRT_SECURE_NO_WARNINGS;_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS;"
    )
    add_compile_definitions(STORMLIB_NO_AUTO_LINK)
    set(STORMLIB_NO_AUTO_LINK ON)
elseif(CMAKE_SYSTEM_NAME STREQUAL "CafeOS")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:_DEBUG>"
        "$<$<CONFIG:Release>:NDEBUG>"
        "SPDLOG_ACTIVE_LEVEL=3;SPDLOG_NO_THREAD_ID;SPDLOG_NO_TLS;STBI_NO_THREAD_LOCALS;"
    )
    target_include_directories(${PROJECT_NAME} PRIVATE ${DEVKITPRO}/portlibs/wiiu/include/)
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU|Clang|AppleClang")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:_DEBUG>"
        "$<$<CONFIG:Release>:NDEBUG>"
        "$<$<BOOL:${BUILD_CROWD_CONTROL}>:ENABLE_CROWD_CONTROL>"
        "SPDLOG_ACTIVE_LEVEL=0;_CONSOLE;_CRT_SECURE_NO_WARNINGS;UNICODE;_UNICODE"
    )
endif()

# ---- Find and Link Extra Libraries Per Platform ----
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    find_package(Ogg CONFIG REQUIRED)
    link_libraries(Ogg::ogg)
    find_package(Vorbis CONFIG REQUIRED)
    link_libraries(Vorbis::vorbisfile)
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<BOOL:${USE_NETWORKING}>:SDL2_net::SDL2_net-static>"
        "Ogg::ogg"
        "Vorbis::vorbis"
        "Vorbis::vorbisenc"
        "Vorbis::vorbisfile"
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
    find_package(SDL2)
    find_package(Vorbis REQUIRED)
    link_libraries(Vorbis::vorbisfile)
    find_package(Ogg REQUIRED)
    link_libraries(Ogg::ogg)
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        -lglad
        SDL2::SDL2
        "Ogg::ogg"
        "Vorbis::vorbisfile"
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "CafeOS")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:Debug>:-Wl,--wrap=abort>"
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Android")
    # ANDROID: do not require or link Ogg/Vorbis (not needed)
    set(ADDITIONAL_LIBRARY_DEPENDENCIES "")
else() # Linux/Unix/Other
    find_package(Ogg REQUIRED)
    find_package(Vorbis REQUIRED)
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<BOOL:${USE_NETWORKING}>:SDL2_net::SDL2_net>"
        "Ogg::ogg"
        "Vorbis::vorbis"
        "Vorbis::vorbisenc"
        "Vorbis::vorbisfile"
    )
endif()

# ---- Networking Option ----
if(USE_NETWORKING)
    if(MSVC)
        vcpkg_install_packages(sdl2-net)
    endif()
    # Only try to find SDL2_net if not Android
    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Android")
        find_package(SDL2_net REQUIRED)
        include_directories(${SDL2_NET_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME} PRIVATE SDL2_net::SDL2_net)
    endif()
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_NETWORKING)
endif()

# ---- Target Link Libraries ----
target_link_libraries(${PROJECT_NAME} PRIVATE libultraship ${ADDITIONAL_LIBRARY_DEPENDENCIES})
add_dependencies(${PROJECT_NAME} libultraship)

# ---- Build Feature Toggles (keep as options for GUI etc) ----
option(USE_STANDALONE "Build as a standalone executable" OFF)
option(BUILD_STORMLIB "Build with StormLib support" OFF)
option(BUILD_SM64 "Build with Super Mario 64 support" OFF)
option(BUILD_MK64 "Build with Mario Kart 64 support" OFF)
option(BUILD_FZERO "Build with F-Zero X support" OFF)

# ---- NintendoSwitch Only: NRO/NACP ----
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

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    # ...your MSVC block is correct...
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    if(CMAKE_SYSTEM_NAME STREQUAL "Darwin" OR CMAKE_SYSTEM_NAME STREQUAL "iOS")
        # ...your Darwin/iOS block is correct...
    elseif(CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
        # ...NintendoSwitch block is correct...
    elseif(CMAKE_SYSTEM_NAME STREQUAL "CafeOS")
        # ...CafeOS block is correct...
    else() # Linux, Android, others
        unset(CPU_OPTION)
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
            set(CPU_OPTION -msse2 -mfpmath=sse)
        elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
            if(NOT CMAKE_SYSTEM_NAME STREQUAL "Android")
                set(CPU_OPTION -mcpu=native)
            endif()
        endif()
        target_compile_options(${PROJECT_NAME} PRIVATE
            -Wall -Wextra -Wno-error
            -Wno-unused-parameter
            -Wno-unused-function
            -Wno-unused-variable
            -Wno-missing-field-initializers
            -Wno-parentheses
            -Wno-narrowing
            -Wno-missing-braces
            -Wno-int-conversion
            $<$<COMPILE_LANGUAGE:C>:
                -Werror-implicit-function-declaration
                -Wno-incompatible-pointer-types
                -fpermissive
            >
            $<$<COMPILE_LANGUAGE:CXX>:-fpermissive>
            $<$<COMPILE_LANGUAGE:CXX>:-Wno-deprecated-enum-enum-conversion>
            -pthread
            ${CPU_OPTION}
        )
        target_link_options(${PROJECT_NAME} PRIVATE -pthread -Wl,-export-dynamic)
    endif()
endif()

# ----------------- Torch/O2R Extraction Only For Non-Android, Non-Switch -------------------
if(NOT (CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch" OR CMAKE_SYSTEM_NAME STREQUAL "Android"))
    include(ExternalProject)
    ExternalProject_Add(TorchExternal
        PREFIX TorchExternal
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/tools/Torch
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/Torch
    )
    ExternalProject_Get_Property(TorchExternal install_dir)
    if (MSVC)
      set(TORCH_EXECUTABLE ${install_dir}/src/TorchExternal-build/$<CONFIGURATION>/torch)
    else()
      set(TORCH_EXECUTABLE ${install_dir}/src/TorchExternal-build/torch)
    endif()

    add_custom_target(
        ExtractAssets
        DEPENDS TorchExternal
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMAND ${TORCH_EXECUTABLE} o2r baserom.z64
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/sf64.o2r" "${CMAKE_BINARY_DIR}/sf64.o2r"
    )

    add_custom_target(
        GeneratePortO2R
        DEPENDS TorchExternal
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMAND ${TORCH_EXECUTABLE} pack port starship.o2r o2r
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/starship.o2r" "${CMAKE_BINARY_DIR}/starship.o2r"
    )

    if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
        install(FILES "${CMAKE_BINARY_DIR}/starship.o2r" DESTINATION . COMPONENT ${PROJECT_NAME})
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

    set_property(TARGET ${PROJECT_NAME} PROPERTY APPIMAGE_DESKTOP_FILE_TERMINAL YES)
    set_property(TARGET ${PROJECT_NAME} PROPERTY APPIMAGE_DESKTOP_FILE "${CMAKE_SOURCE_DIR}/Starship.desktop")
    set_property(TARGET ${PROJECT_NAME} PROPERTY APPIMAGE_ICON_FILE "${CMAKE_SOURCE_DIR}/logo.png")
endif()

# ---- Packaging (skip for Android) ----
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(CPACK_GENERATOR "External")
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows|NintendoSwitch|CafeOS")
    set(CPACK_GENERATOR "ZIP")
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(CPACK_GENERATOR "Bundle")
endif()

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Android")
    set(CPACK_PROJECT_CONFIG_FILE ${CMAKE_SOURCE_DIR}/cmake/configure-packaging.cmake)
    include(cmake/packaging.cmake)
endif()
