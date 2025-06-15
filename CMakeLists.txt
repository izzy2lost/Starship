# Android-specific setup
if(ANDROID)
    set(ANDROID_STL c++_shared)
    set(ANDROID_PLATFORM android-24)
    add_definitions(-DANDROID -DPLATFORM_ANDROID=1)

    # Android requires shared library
    add_library(${PROJECT_NAME} SHARED ${ALL_FILES})

    # Copy assets to build directory
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets/"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets/"
    )
else()
    add_executable(${PROJECT_NAME} ${ALL_FILES})
endif()

# ...

# Library linking for Android
if(ANDROID)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        GLESv2
        EGL
        log
        android
        libultraship
        # Add other dependencies here
    )
endif()

# ...

# Platform-specific library dependencies
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Windows config
elseif(CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
    # Switch config
elseif(CMAKE_SYSTEM_NAME STREQUAL "CafeOS")
    # Wii U config
elseif(ANDROID)
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        GLESv2
        EGL
        log
        android
    )
else()
    # Linux/macOS config
endif()
