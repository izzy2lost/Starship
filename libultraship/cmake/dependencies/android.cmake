include(FetchContent)

# =========== bzip2 ===========
FetchContent_Declare(
    bzip2
    GIT_REPOSITORY https://github.com/libarchive/bzip2.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(bzip2)

#=================== SDL2 ===================
find_package(SDL2 QUIET)
if (NOT ${SDL2_FOUND})
    FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-2.32.8
        OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(SDL2)
endif()

# =========== SDL2_net ===========
find_package(SDL2_net QUIET)
if (NOT SDL2_NET_FOUND)
    FetchContent_Declare(
        SDL2_net
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_net.git
        GIT_TAG release-2.2.0
        OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(SDL2_net)
endif()

# =========== libpng ===========
find_package(PNG QUIET)
if (NOT PNG_FOUND)
    FetchContent_Declare(
        libpng
        GIT_REPOSITORY https://github.com/glennrp/libpng.git
        GIT_TAG v1.6.43
        OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(libpng)
endif()

#=================== nlohmann-json ===================
find_package(nlohmann_json QUIET)
if (NOT ${nlohmann_json_FOUND})
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
        OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

#=================== tinyxml2 ===================
find_package(tinyxml2 QUIET)
if (NOT ${tinyxml2_FOUND})
    set(tinyxml2_BUILD_TESTING OFF)
    FetchContent_Declare(
        tinyxml2
        GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
        GIT_TAG 10.0.0
        OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(tinyxml2)
endif()

#=================== spdlog ===================
find_package(spdlog QUIET)
if (NOT ${spdlog_FOUND})
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.14.1
        OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(spdlog)
endif()

# =========== libzip (with all optional compression!) ===========
find_package(libzip QUIET)
if (NOT libzip_FOUND)
    set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
    set(BUILD_TOOLS OFF)
    set(BUILD_REGRESS OFF)
    set(BUILD_EXAMPLES OFF)
    set(BUILD_DOC OFF)
    set(BUILD_OSSFUZZ OFF)
    set(BUILD_SHARED_LIBS OFF)
    set(BZIP2_LIBRARY ${bzip2_BINARY_DIR}/libbz2.a)
    set(BZIP2_INCLUDE_DIR ${bzip2_SOURCE_DIR})
    set(ENABLE_BZIP2 ON)
    FetchContent_Declare(
        libzip
        GIT_REPOSITORY https://github.com/nih-at/libzip.git
        GIT_TAG v1.10.1
        OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(libzip)
    list(APPEND ADDITIONAL_LIB_INCLUDES ${libzip_SOURCE_DIR}/lib ${libzip_BINARY_DIR})
endif()

target_link_libraries(ImGui PUBLIC SDL2::SDL2)
