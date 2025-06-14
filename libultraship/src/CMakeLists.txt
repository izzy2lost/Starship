cmake_minimum_required(VERSION 3.16...3.31.5)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)

# ========== Platform-specific sources ===========
set(LIBUTRASHIP_PORT_SOURCES "")

if(CMAKE_SYSTEM_NAME STREQUAL "CafeOS")
    set(LIBUTRASHIP_PORT_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/port/wiiu/WiiUController.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/port/wiiu/WiiUGamepad.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/port/wiiu/WiiUImpl.cpp
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
    set(LIBUTRASHIP_PORT_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/port/switch/SwitchImpl.cpp
        # SwitchPerformanceProfiles.h is a header, omit here
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Android")
    set(LIBUTRASHIP_PORT_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/port/android/AndroidImpl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/port/android/os_stub.c
    )
endif()

# Always have a dummy file for empty builds
if(LIBUTRASHIP_PORT_SOURCES STREQUAL "")
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/dummy.c "// dummy for libultraship")
    set(LIBUTRASHIP_PORT_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/dummy.c)
endif()

# =========== Main source file sets ===========
set(Source_Files__TopLevel
    ${CMAKE_CURRENT_SOURCE_DIR}/Context.cpp
)
set(Source_Files__Audio
    ${CMAKE_CURRENT_SOURCE_DIR}/audio/Audio.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/audio/AudioPlayer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/audio/SDLAudioPlayer.cpp
)
set(Source_Files__Controller
    ${CMAKE_CURRENT_SOURCE_DIR}/controller/ControlDeck.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/controller/Controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/controller/DummyController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/controller/KeyboardController.cpp
)
if (NOT CMAKE_SYSTEM_NAME STREQUAL "CafeOS")
    list(APPEND Source_Files__Controller
        ${CMAKE_CURRENT_SOURCE_DIR}/controller/SDLController.cpp
    )
endif()
set(Source_Files__Config
    ${CMAKE_CURRENT_SOURCE_DIR}/config/ConsoleVariable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/config/Config.cpp
)
set(Source_Files__Public__Libultra
    ${CMAKE_CURRENT_SOURCE_DIR}/public/libultra/os.cpp
)
set(Source_Files__Public__Bridge
    ${CMAKE_CURRENT_SOURCE_DIR}/public/bridge/resourcebridge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/public/bridge/windowbridge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/public/bridge/audiobridge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/public/bridge/controllerbridge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/public/bridge/consolevariablebridge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/public/bridge/crashhandlerbridge.cpp
)
set(Source_Files__Debug
    ${CMAKE_CURRENT_SOURCE_DIR}/debug/CrashHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/debug/Console.cpp
)
set(Source_Files__Log
    ${CMAKE_CURRENT_SOURCE_DIR}/log/luslog.cpp
)
set(Source_Files__Window
    ${CMAKE_CURRENT_SOURCE_DIR}/window/Window.cpp
)
set(Source_Files__Window__Gui
    ${CMAKE_CURRENT_SOURCE_DIR}/window/gui/ConsoleWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/window/gui/Gui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/window/gui/InputEditorWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/window/gui/InputViewer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/window/gui/GameOverlay.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/window/gui/StatsWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/window/gui/GuiWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/window/gui/GuiElement.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/window/gui/GuiMenuBar.cpp
)
set(Source_Files__Utils
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/stox.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/Utils.cpp
)
set(Source_Files__Utils__BinaryTools
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/binarytools/BinaryReader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/binarytools/BinaryWriter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/binarytools/MemoryStream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/binarytools/Stream.cpp
)
set(Source_Files__Resource
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/Archive.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/Resource.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/ResourceManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/ResourceLoader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/ResourceFactory.cpp
)
set(Source_Files__Resource__Types
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/type/Array.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/type/Blob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/type/DisplayList.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/type/Matrix.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/type/Texture.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/type/Vertex.cpp
)
set(Source_Files__Resource__Factories
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/factory/ArrayFactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/factory/BlobFactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/factory/DisplayListFactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/factory/MatrixFactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/factory/TextureFactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/factory/VertexFactory.cpp
)
set(Source_Files__Graphic
    ${CMAKE_CURRENT_SOURCE_DIR}/graphic/Fast3D/gfx_cc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graphic/Fast3D/gfx_pc.cpp
)
set(Source_Files__Lib__stb
    "${CMAKE_CURRENT_SOURCE_DIR}/../extern/stb/stb_impl.c"
)

# ============ Add all sources to the target ============
add_library(libultraship STATIC
    ${Source_Files__TopLevel}
    ${Source_Files__Audio}
    ${Source_Files__Controller}
    ${Source_Files__Config}
    ${Source_Files__Public__Libultra}
    ${Source_Files__Public__Bridge}
    ${Source_Files__Debug}
    ${Source_Files__Log}
    ${Source_Files__Window}
    ${Source_Files__Window__Gui}
    ${Source_Files__Utils}
    ${Source_Files__Utils__BinaryTools}
    ${LIBUTRASHIP_PORT_SOURCES}
    ${Source_Files__Resource}
    ${Source_Files__Resource__Types}
    ${Source_Files__Resource__Factories}
    ${Source_Files__Graphic}
    ${Source_Files__Lib__stb}
)
set_target_properties(libultraship PROPERTIES PREFIX "")

# ================== Include directories ==================
target_include_directories(libultraship
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../extern
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC ${INCLUDE_DIR}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../extern/spdlog/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../extern/stb
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../extern/nlohmann-json/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../extern/nlohmann-json/single_include
)

# ========== Platform-specific compile defs & links ==========
if (CMAKE_SYSTEM_NAME STREQUAL "CafeOS")
    target_include_directories(libultraship PRIVATE
        ${DEVKITPRO}/portlibs/wiiu/include/
    )
    target_compile_definitions(libultraship PRIVATE
        SPDLOG_NO_THREAD_ID SPDLOG_NO_TLS STBI_NO_THREAD_LOCALS SPDLOG_ACTIVE_LEVEL=3
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
    )
elseif (NOT CMAKE_SYSTEM_NAME STREQUAL "CafeOS")
    target_compile_definitions(libultraship PRIVATE
        ENABLE_OPENGL
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
        $<$<CONFIG:Debug>:SPDLOG_ACTIVE_LEVEL=0>
        $<$<NOT:$<CONFIG:Debug>>:SPDLOG_ACTIVE_LEVEL=1>
        $<$<BOOL:${USE_OPENGLES}>:USE_OPENGLES>
        "-DANDROID_APPNAME=\"${ANDROID_APPNAME}\""
    )
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Android")
    target_link_libraries(libultraship PRIVATE log)
endif()

if (USE_OPENGLES)
    find_library(OPENGLES_LIBRARY GLESv3)
    target_link_libraries(libultraship PRIVATE GLESv3)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin" OR CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(libultraship PRIVATE Threads::Threads)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    find_library(OSX_FOUNDATION Foundation)
    find_library(OSX_AVFOUNDATION AVFoundation)
    find_library(METAL Metal)
    find_library(QUARTZCORE QuartzCore)
    target_link_libraries(libultraship PRIVATE ${OSX_FOUNDATION} ${OSX_AVFOUNDATION} ${METAL} ${QUARTZCORE} ${CMAKE_DL_LIBS})
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(PulseAudio)
    if(PULSEAUDIO_LIBRARY)
        target_link_libraries(libultraship PRIVATE ${PULSEAUDIO_LIBRARY})
    endif()
endif()

if (MSVC)
    target_compile_options(libultraship PRIVATE
        $<$<CONFIG:Debug>:/Od;/Oi->
        $<$<CONFIG:Release>:/Oi;/Gy>
        /permissive-;/MP;/sdl;/W3;
    )
    target_link_options(libultraship PRIVATE
        $<$<CONFIG:Release>:/OPT:REF;/OPT:ICF>
        /SUBSYSTEM:CONSOLE
    )
endif()

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU|Clang")
    target_compile_options(libultraship PRIVATE
        -Wall -Wextra -Wno-error -Wno-unused-variable -Wno-unused-parameter
        -Wno-unused-function -Wno-parentheses -Wno-narrowing -Wno-missing-field-initializers
        $<$<COMPILE_LANGUAGE:C,OBJC>:-Wno-int-conversion>
    )
endif()
