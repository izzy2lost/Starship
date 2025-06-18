cmake_minimum_required(VERSION 3.12)
project(torch)
include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

# Set build options
option(USE_STANDALONE "Build as a standalone executable" ON)
option(BUILD_STORMLIB "Build with StormLib support" ON)
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)

option(BUILD_SM64 "Build with Super Mario 64 support" ON)
option(BUILD_MK64 "Build with Mario Kart 64 support" ON)
option(BUILD_SF64 "Build with Star Fox 64 support" ON)
option(BUILD_FZERO "Build with F-Zero X support" ON)
option(BUILD_NAUDIO "Build with NAudio support" ON)

################################################################################
# Set target arch type if empty. Visual studio solution generator provides it.
################################################################################
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if(NOT CMAKE_VS_PLATFORM_NAME)
        set(CMAKE_VS_PLATFORM_NAME "x64")
    endif()
    message("${CMAKE_VS_PLATFORM_NAME} architecture in use")

    if(NOT ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64"
        OR "${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32"))
        message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
    endif()
endif()

if(USE_STANDALONE)
    # Link libgfxd
    # Because libgfxd is not a CMake project, we have to manually fetch it and add it to the build
    FetchContent_Declare(
        libgfxd
        GIT_REPOSITORY https://github.com/glankk/libgfxd.git
        GIT_TAG 96fd3b849f38b3a7c7b7f3ff03c5921d328e6cdf
    )

    FetchContent_GetProperties(libgfxd)

    if(NOT libgfxd_POPULATED)
        FetchContent_MakeAvailable(libgfxd)
        include_directories(${libgfxd_SOURCE_DIR})
        set(LGFXD_SRC gfxd.c uc_f3d.c uc_f3db.c uc_f3dex.c uc_f3dexb.c uc_f3dex2.c)
        foreach (LGFXD_FILE ${LGFXD_SRC})
            list(APPEND LGFXD_FILES "${libgfxd_SOURCE_DIR}/${LGFXD_FILE}")
        endforeach()
    endif()
endif()
# Source files

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB_RECURSE CXX_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/lib/strhash64/*.cpp)
file(GLOB C_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c ${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.c ${CMAKE_CURRENT_SOURCE_DIR}/lib/**/*.c)
set(SRC_DIR ${CXX_FILES} ${C_FILES} ${LGFXD_FILES})

if(BUILD_SM64)
    add_definitions(-DSM64_SUPPORT)
else()
    list(FILTER SRC_DIR EXCLUDE REGEX "${CMAKE_CURRENT_SOURCE_DIR}/src/factories/sm64/*")
endif()

if(BUILD_SF64)
    add_definitions(-DSF64_SUPPORT)
else()
    list(FILTER SRC_DIR EXCLUDE REGEX "${CMAKE_CURRENT_SOURCE_DIR}/src/factories/sf64/*")
endif()

if(BUILD_MK64)
    add_definitions(-DMK64_SUPPORT)
else()
    list(FILTER SRC_DIR EXCLUDE REGEX "${CMAKE_CURRENT_SOURCE_DIR}/src/factories/mk64/*")
endif()

if(BUILD_FZERO)
    add_definitions(-DFZERO_SUPPORT)
else()
    list(FILTER SRC_DIR EXCLUDE REGEX "${CMAKE_CURRENT_SOURCE_DIR}/src/factories/fzerox/*")
endif()

if(BUILD_NAUDIO)
    add_definitions(-DNAUDIO_SUPPORT)
else()
    list(FILTER SRC_DIR EXCLUDE REGEX "${CMAKE_CURRENT_SOURCE_DIR}/src/factories/naudio/*")
endif()

if(ENABLE_ASAN)
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
endif()

# Build
if (USE_STANDALONE)
    add_definitions(-DSTANDALONE)
    add_executable(${PROJECT_NAME} ${SRC_DIR})
	set(LINK_TYPE "MD")
else()
    add_library(${PROJECT_NAME} STATIC ${SRC_DIR})
	set(LINK_TYPE "MT")
endif()

if (BUILD_STORMLIB)
    add_definitions(-DUSE_STORMLIB)
endif()

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    set(CMAKE_CXX_FLAGS "-Wno-narrowing")
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /w;
            /Od;
            /ZI;
            /${LINK_TYPE}d
        >
        $<$<CONFIG:Release>:
            /O3;
            /Gy;
            /W3;
            /${LINK_TYPE}
        >
        /bigobj
        /permissive-;
        /MP;
        ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
        ${DEFAULT_CXX_EXCEPTION_HANDLING}
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /INCREMENTAL
        >
        $<$<CONFIG:Release>:
            /OPT:REF;
            /OPT:ICF;
            /INCREMENTAL:NO;
            /FORCE:MULTIPLE
        >
        /bigobj;
        /MANIFEST:NO;
        /DEBUG;
        /SUBSYSTEM:CONSOLE
    )

    add_definitions(-DSTORMLIB_NO_AUTO_LINK)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()


# Fetch Dependencies

if(NOT USE_STANDALONE AND EXISTS "/mnt/c/WINDOWS/system32/wsl.exe")
    FetchContent_Declare(
        GSL
        GIT_REPOSITORY https://github.com/Microsoft/GSL.git
        GIT_TAG a3534567187d2edc428efd3f13466ff75fe5805c
    )
    FetchContent_MakeAvailable(GSL)
    target_link_libraries(${PROJECT_NAME} PRIVATE GSL)
endif()

# Link BinaryTools

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/binarytools)
add_dependencies(${PROJECT_NAME} BinaryTools)

# Link n64graphics

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/n64graphics)
add_dependencies(${PROJECT_NAME} N64Graphics)

# Link StormLib
if (BUILD_STORMLIB)
    set(STORM_BUILD_TESTS OFF)
    set(STORMLIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/StormLib)
    add_subdirectory(${STORMLIB_DIR})
    if(USE_STANDALONE)
        if((CMAKE_SYSTEM_NAME MATCHES "Windows") AND ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"))
            include(../cmake/HandleCompilerRT.cmake)
            find_compiler_rt_library(builtins CLANG_RT_BUILTINS_LIBRARY)
            get_filename_component(LIBDIR "${CLANG_RT_BUILTINS_LIBRARY}" DIRECTORY)
            if(USE_STANDALONE AND IS_DIRECTORY "${LIBDIR}")
                target_link_libraries(storm ${CLANG_RT_BUILTINS_LIBRARY})
            endif()
        else()
            target_link_libraries(${PROJECT_NAME} PRIVATE storm)
        endif()
        if (CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
            target_compile_definitions(storm PRIVATE -D_POSIX_C_SOURCE=200809L)
        endif()
    endif ()
endif()

# Link YamlCpp

set(YAML_CPP_BUILD_TOOLS OFF)
set(YAML_CPP_BUILD_TESTS OFF)
set(YAML_CPP_DISABLE_UNINSTALL ON)
FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG 28f93bdec6387d42332220afa9558060c8016795
)
set(YAML_CPP_BUILD_TESTS OFF)
FetchContent_MakeAvailable(yaml-cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE yaml-cpp)
if(NOT USE_STANDALONE)
	target_compile_definitions(yaml-cpp PRIVATE YAML_CPP_STATIC_DEFINE)
endif()

if(USE_STANDALONE)
	FetchContent_Declare(
		spdlog
		GIT_REPOSITORY https://github.com/gabime/spdlog.git
		GIT_TAG 7e635fca68d014934b4af8a1cf874f63989352b7
	)

	FetchContent_MakeAvailable(spdlog)
	target_link_libraries(${PROJECT_NAME} PRIVATE spdlog)
else()
	find_package(spdlog REQUIRED)
	target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)
endif()

# Link TinyXML2
set(tinyxml2_BUILD_TESTING OFF)
FetchContent_Declare(
    tinyxml2
    GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
    GIT_TAG 10.0.0
    OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(tinyxml2)

target_link_libraries(${PROJECT_NAME} PRIVATE tinyxml2 yaml-cpp N64Graphics BinaryTools)

if(NOT USE_STANDALONE)
    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)
    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
    target_include_directories(${PROJECT_NAME} PUBLIC ${yaml-cpp_SOURCE_DIR}/include)
endif()
