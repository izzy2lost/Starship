include(FetchContent)

##=================== SDL2 ===================
find_package(SDL2 QUIET)
if (NOT ${SDL2_FOUND})
    FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-2.28.1
        OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(SDL2)
    if(EXISTS "${sdl2_SOURCE_DIR}/src/sensor/android/SDL_androidsensor.c")
        file(READ "${sdl2_SOURCE_DIR}/src/sensor/android/SDL_androidsensor.c" _sdlfile)
        string(REPLACE "ALooper_pollAll" "ALooper_pollOnce" _sdlfile "${_sdlfile}")
        file(WRITE "${sdl2_SOURCE_DIR}/src/sensor/android/SDL_androidsensor.c" "${_sdlfile}")
    endif()
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

#=================== libzip ===================
find_package(libzip QUIET)
if (NOT ${libzip_FOUND})
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
        GIT_TAG v1.10.1
        OVERRIDE_FIND_PACKAGE
        CMAKE_ARGS
            -DENABLE_COMMONCRYPTO=OFF
            -DENABLE_GNUTLS=OFF
            -DENABLE_MBEDTLS=OFF
            -DENABLE_OPENSSL=OFF
            -DBUILD_TOOLS=OFF
            -DBUILD_REGRESS=OFF
            -DBUILD_EXAMPLES=OFF
            -DBUILD_DOC=OFF
            -DBUILD_OSSFUZZ=OFF
            -DBUILD_SHARED_LIBS=OFF
    )
    FetchContent_MakeAvailable(libzip)
    list(APPEND ADDITIONAL_LIB_INCLUDES ${libzip_SOURCE_DIR}/lib ${libzip_BINARY_DIR})
endif()

target_link_libraries(ImGui PUBLIC SDL2::SDL2)
