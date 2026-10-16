# Third-party dependencies for the Android build.
#
# Every other platform gets these from a package manager — vcpkg on Windows, the
# distro on Linux, Homebrew on macOS, devkitPro on Switch. The NDK build has no
# such source, and libultraship's own cmake/dependencies/android.cmake only
# fetches SDL2, so the rest are fetched here.
#
# This has to run before add_subdirectory(libultraship): OVERRIDE_FIND_PACKAGE
# is what makes libultraship's find_package(<name> REQUIRED) calls resolve to
# these instead of failing.

include(FetchContent)

# =================================== SDL2 ===================================
# Declared here rather than left to libultraship's own android.cmake, which asks
# for release-2.28.1. That version calls ALooper_pollAll, which NDK 27 removed
# from the headers ("obsoleted in Android 1"), so it cannot compile against the
# NDK this app builds with. FetchContent honours the first declaration of a
# name, so declaring it before add_subdirectory(libultraship) is what pins it.
#
# Keep in step with SDL_MICRO_VERSION in
# android/app/src/main/java/org/libsdl/app/SDLActivity.java: SDLActivity refuses
# to start when the Java glue and libSDL2.so disagree on their version.
find_package(SDL2 QUIET)
if(NOT SDL2_FOUND)
    FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-2.32.10
        GIT_SHALLOW TRUE
        OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(SDL2)
endif()

# ============================== nlohmann-json ==============================
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    set(JSON_BuildTests OFF)
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.12.0
        GIT_SHALLOW TRUE
        OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

# ================================= tinyxml2 =================================
find_package(tinyxml2 QUIET)
if(NOT tinyxml2_FOUND)
    set(tinyxml2_BUILD_TESTING OFF)
    FetchContent_Declare(
        tinyxml2
        GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
        GIT_TAG 11.0.0
        GIT_SHALLOW TRUE
        OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(tinyxml2)
endif()

# ================================== spdlog ==================================
find_package(spdlog QUIET)
if(NOT spdlog_FOUND)
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.16.0
        GIT_SHALLOW TRUE
        OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(spdlog)
endif()

# ================================== libzip ==================================
find_package(libzip QUIET)
if(NOT libzip_FOUND)
    # libzip reads these as plain variables rather than cache entries.
    set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
    set(BUILD_TOOLS OFF)
    set(BUILD_REGRESS OFF)
    set(BUILD_EXAMPLES OFF)
    set(BUILD_DOC OFF)
    set(BUILD_OSSFUZZ OFF)
    set(BUILD_SHARED_LIBS OFF)
    FetchContent_Declare(
        libzip
        GIT_REPOSITORY https://github.com/nih-at/libzip.git
        GIT_TAG v1.11.4
        GIT_SHALLOW TRUE
        OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(libzip)

    # libultraship links libzip PRIVATE, so its include directories stop there —
    # but O2rArchive.h includes <zip.h> and the game includes that header, so the
    # game needs them too. Every other platform gets zip.h from a system include
    # path and never notices. zipconf.h is generated, hence the binary dir.
    include_directories(${libzip_SOURCE_DIR}/lib ${libzip_BINARY_DIR})
endif()

# ============================== Ogg and Vorbis ==============================
# The Android branch of the platform switch links Ogg::ogg and Vorbis::* but,
# unlike every other platform there, never had a find_package to satisfy them.
find_package(Ogg QUIET)
if(NOT Ogg_FOUND)
    FetchContent_Declare(
        Ogg
        GIT_REPOSITORY https://github.com/xiph/ogg.git
        GIT_TAG v1.3.5
        GIT_SHALLOW TRUE
        OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(Ogg)
endif()

find_package(Vorbis QUIET)
if(NOT Vorbis_FOUND)
    FetchContent_Declare(
        Vorbis
        GIT_REPOSITORY https://github.com/xiph/vorbis.git
        GIT_TAG v1.3.7
        GIT_SHALLOW TRUE
        OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(Vorbis)

    # Unlike ogg, which aliases Ogg::ogg in its own CMakeLists, vorbis only
    # defines the Vorbis:: names in the config package it installs. Built as a
    # subproject that never runs, so the callers linking Vorbis::vorbis would
    # fail on a target that does not exist.
    foreach(component vorbis vorbisenc vorbisfile)
        if(TARGET ${component} AND NOT TARGET Vorbis::${component})
            add_library(Vorbis::${component} ALIAS ${component})
        endif()
    endforeach()
endif()
