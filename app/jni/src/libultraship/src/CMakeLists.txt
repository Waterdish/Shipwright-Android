add_library(libultraship STATIC)
set_target_properties(libultraship PROPERTIES PREFIX "")
set_property(TARGET libultraship PROPERTY CXX_STANDARD 20)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)

find_library(OPENGL_LIBRARY GLESv3)
include_directories(include ${OPENGL_INCLUDE_DIRS})
#target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_LIBRARY})
#noted. don't link to project_name. Link below to libultraship.

#find_library(OPENGL_LIBRARY gl4es)
#include_directories(include ${OPENGL_INCLUDE_DIRS})
#target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_LIBRARY})

#find_library(EGL_LIBRARY EGL)
#include_directories(include ${EGL_INCLUDE_DIRS})
#target_link_libraries(${PROJECT_NAME} PRIVATE ${EGL_LIBRARY})

#i might also want to specify other stuff like loader

#use this if using gl4es
#target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../gl4es/include)



#target_link_libraries(${PROJECT_NAME} PRIVATE GL4ES)

#gl4es
#target_link_libraries(libultraship GL) #can't do this twice. do it below
#target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../gl4es/include)
#add_library(gl4es SHARED IMPORTED)
#set_target_properties(gl4es PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../../../lib/x86_64/libGL4ES.so)
#target_link_libraries(${PROJECT_NAME} PRIVATE gl4es)

#target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../gl4es/include)
#target_link_libraries(${PROJECT_NAME} PRIVATE GL)
#target_link_libraries(${PROJECT_NAME} PRIVATE GLEW)

#target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../gl4eslib/libgl4es_114.so)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
endif()

#=================== Top-Level ===================

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/install_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/install_config.h @ONLY)

set(Source_Files__TopLevel
    ${CMAKE_CURRENT_SOURCE_DIR}/Context.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Context.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/install_config.h
)

source_group("" FILES ${Source_Files__TopLevel})
target_sources(libultraship PRIVATE ${Source_Files__TopLevel})

#=================== Audio ===================

set(Source_Files__Audio
    ${CMAKE_CURRENT_SOURCE_DIR}/audio/Audio.h
    ${CMAKE_CURRENT_SOURCE_DIR}/audio/Audio.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/audio/AudioPlayer.h
	${CMAKE_CURRENT_SOURCE_DIR}/audio/AudioPlayer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/audio/SDLAudioPlayer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/audio/SDLAudioPlayer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/audio/PulseAudioPlayer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/audio/PulseAudioPlayer.cpp
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND Source_Files__Audio ${CMAKE_CURRENT_SOURCE_DIR}/audio/WasapiAudioPlayer.h)
    list(APPEND Source_Files__Audio ${CMAKE_CURRENT_SOURCE_DIR}/audio/WasapiAudioPlayer.cpp)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND Source_Files__Audio ${CMAKE_CURRENT_SOURCE_DIR}/audio/PulseAudioPlayer.h)
    list(APPEND Source_Files__Audio ${CMAKE_CURRENT_SOURCE_DIR}/audio/PulseAudioPlayer.cpp)
endif()

source_group("audio" FILES ${Source_Files__Audio})
target_sources(libultraship PRIVATE ${Source_Files__Audio})

#=================== Controller ===================

set(Source_Files__Controller
    ${CMAKE_CURRENT_SOURCE_DIR}/controller/ControlDeck.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/controller/ControlDeck.h
    ${CMAKE_CURRENT_SOURCE_DIR}/controller/Controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/controller/Controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/controller/DummyController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/controller/DummyController.h
    ${CMAKE_CURRENT_SOURCE_DIR}/controller/KeyboardController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/controller/KeyboardController.h
)

if (NOT CMAKE_SYSTEM_NAME STREQUAL "CafeOS")
    list(APPEND Source_Files__Controller
        ${CMAKE_CURRENT_SOURCE_DIR}/controller/SDLController.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/controller/SDLController.h
    )
endif()

source_group("controller" FILES ${Source_Files__Controller})
target_sources(libultraship PRIVATE ${Source_Files__Controller})

#=================== Config ===================

set(Source_Files__Config
    ${CMAKE_CURRENT_SOURCE_DIR}/config/ConsoleVariable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/config/ConsoleVariable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/config/Config.h
    ${CMAKE_CURRENT_SOURCE_DIR}/config/Config.cpp
)

source_group("config" FILES ${Source_Files__Config})
target_sources(libultraship PRIVATE ${Source_Files__Config})

#=================== Public ===================

source_group("public")

set(Source_Files__Public__Libultra
    ${CMAKE_CURRENT_SOURCE_DIR}/public/libultra/os.h
    ${CMAKE_CURRENT_SOURCE_DIR}/public/libultra/os.cpp
)
 
source_group("public/libultra" FILES ${Source_Files__Public__Libultra})

set(Source_Files__Public__Bridge
    ${CMAKE_CURRENT_SOURCE_DIR}/public/bridge/resourcebridge.h
    ${CMAKE_CURRENT_SOURCE_DIR}/public/bridge/resourcebridge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/public/bridge/windowbridge.h
    ${CMAKE_CURRENT_SOURCE_DIR}/public/bridge/windowbridge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/public/bridge/audiobridge.h
    ${CMAKE_CURRENT_SOURCE_DIR}/public/bridge/audiobridge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/public/bridge/controllerbridge.h
    ${CMAKE_CURRENT_SOURCE_DIR}/public/bridge/controllerbridge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/public/bridge/consolevariablebridge.h
    ${CMAKE_CURRENT_SOURCE_DIR}/public/bridge/consolevariablebridge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/public/bridge/crashhandlerbridge.h
    ${CMAKE_CURRENT_SOURCE_DIR}/public/bridge/crashhandlerbridge.cpp
)
 
source_group("public/bridge" FILES ${Source_Files__Public__Bridge})
target_sources(libultraship PRIVATE ${Source_Files__Public__Libultra} ${Source_Files__Public__Bridge})

#=================== Debug ===================

set(Source_Files__Debug
    ${CMAKE_CURRENT_SOURCE_DIR}/debug/CrashHandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/debug/CrashHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/debug/Console.h
    ${CMAKE_CURRENT_SOURCE_DIR}/debug/Console.cpp
)

source_group("debug" FILES ${Source_Files__Debug})
target_sources(libultraship PRIVATE ${Source_Files__Debug})

#=================== Log ===================

set(Source_Files__Log
    ${CMAKE_CURRENT_SOURCE_DIR}/log/luslog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/log/luslog.cpp
)

source_group("Log" FILES ${Source_Files__Log})

target_sources(libultraship PRIVATE ${Source_Files__Log} ${Source_Files__Log__SPDLog})

#=================== Window ===================
set(Source_Files__Window
    ${CMAKE_CURRENT_SOURCE_DIR}/window/Window.h
    ${CMAKE_CURRENT_SOURCE_DIR}/window/Window.cpp
)

source_group("window" FILES ${Source_Files__Window})
target_sources(libultraship PRIVATE ${Source_Files__Window})

#=================== Gui ===================

set(Source_Files__Window__Gui
    ${CMAKE_CURRENT_SOURCE_DIR}/window/gui/ConsoleWindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/window/gui/ConsoleWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/window/gui/Gui.h
    ${CMAKE_CURRENT_SOURCE_DIR}/window/gui/Gui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/window/gui/InputEditorWindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/window/gui/InputEditorWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/window/gui/InputViewer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/window/gui/InputViewer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/window/gui/GameOverlay.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/window/gui/GameOverlay.h
    ${CMAKE_CURRENT_SOURCE_DIR}/window/gui/Fonts.h
    ${CMAKE_CURRENT_SOURCE_DIR}/window/gui/IconsFontAwesome4.h
    ${CMAKE_CURRENT_SOURCE_DIR}/window/gui/StatsWindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/window/gui/StatsWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/window/gui/GuiWindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/window/gui/GuiWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/window/gui/GuiElement.h
    ${CMAKE_CURRENT_SOURCE_DIR}/window/gui/GuiElement.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/window/gui/GuiMenuBar.h
    ${CMAKE_CURRENT_SOURCE_DIR}/window/gui/GuiMenuBar.cpp
)

source_group("window/gui" FILES ${Source_Files__Window__Gui})
target_sources(libultraship PRIVATE ${Source_Files__Window__Gui})

#=================== Utils ===================

set(Source_Files__Utils
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/stox.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/stox.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/Utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/Utils.cpp
)

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    list(APPEND Source_Files__Utils
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/OSXFolderManager.h
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/OSXFolderManager.mm
    )

    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/utils/OSXFolderManager.mm PROPERTIES COMPILE_FLAGS -fno-objc-arc)
endif()

source_group("utils" FILES ${Source_Files__Utils})
target_sources(libultraship PRIVATE ${Source_Files__Utils})

#=================== BinaryTools ==================

set(Source_Files__Utils__BinaryTools
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/binarytools/BinaryReader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/binarytools/BinaryReader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/binarytools/BinaryWriter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/binarytools/BinaryWriter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/binarytools/endianness.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/binarytools/MemoryStream.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/binarytools/MemoryStream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/binarytools/Stream.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/binarytools/Stream.cpp
)

source_group("utils/binarytools" FILES ${Source_Files__Utils__BinaryTools})
target_sources(libultraship PRIVATE ${Source_Files__Utils__BinaryTools})

#=================== Port ===================

if (CMAKE_SYSTEM_NAME STREQUAL "CafeOS")
    set(Source_Files__Port
        ${CMAKE_CURRENT_SOURCE_DIR}/port/wiiu/WiiUController.h
        ${CMAKE_CURRENT_SOURCE_DIR}/port/wiiu/WiiUController.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/port/wiiu/WiiUGamepad.h
        ${CMAKE_CURRENT_SOURCE_DIR}/port/wiiu/WiiUGamepad.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/port/wiiu/WiiUImpl.h
        ${CMAKE_CURRENT_SOURCE_DIR}/port/wiiu/WiiUImpl.cpp
    )
elseif (CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
    set(Source_Files__Port
        ${CMAKE_CURRENT_SOURCE_DIR}/port/switch/SwitchImpl.h
        ${CMAKE_CURRENT_SOURCE_DIR}/port/switch/SwitchImpl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/port/switch/SwitchPerformanceProfiles.h
    )
endif()

source_group("port" FILES ${Source_Files__Port})
target_sources(libultraship PRIVATE ${Source_Files__Port})

#=================== Resource ===================

set(Source_Files__Resource
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/Archive.h
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/Archive.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/File.h
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/Resource.h
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/ResourceType.h
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/Resource.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/ResourceManager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/ResourceManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/ResourceLoader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/ResourceLoader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/ResourceFactory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/ResourceFactory.cpp
)
source_group("resource" FILES ${Source_Files__Resource})

set(Source_Files__Resource__Types
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/type/Array.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/type/Array.h
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/type/Blob.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/type/Blob.h
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/type/DisplayList.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/type/DisplayList.h
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/type/Matrix.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/type/Matrix.h
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/type/Texture.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/type/Texture.h
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/type/Vertex.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/type/Vertex.h
)
source_group("resource/type" FILES ${Source_Files__Resource__Types})

set(Source_Files__Resource__Factories
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/factory/ArrayFactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/factory/ArrayFactory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/factory/BlobFactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/factory/BlobFactory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/factory/DisplayListFactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/factory/DisplayListFactory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/factory/MatrixFactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/factory/MatrixFactory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/factory/TextureFactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/factory/TextureFactory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/factory/VertexFactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/factory/VertexFactory.h
)
source_group("resource/factory" FILES ${Source_Files__Resource__Factories})
target_sources(libultraship PRIVATE ${Source_Files__Resource} ${Source_Files__Resource__Types} ${Source_Files__Resource__Factories})

#=================== Graphic ===================

set(Source_Files__Graphic
    ${CMAKE_CURRENT_SOURCE_DIR}/graphic/Fast3D/gfx_cc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/graphic/Fast3D/gfx_cc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graphic/Fast3D/gfx_pc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/graphic/Fast3D/gfx_pc.cpp
)

if (NOT CMAKE_SYSTEM_NAME STREQUAL "CafeOS")
    list(APPEND Source_Files__Graphic
        ${CMAKE_CURRENT_SOURCE_DIR}/graphic/Fast3D/gfx_opengl.h
        ${CMAKE_CURRENT_SOURCE_DIR}/graphic/Fast3D/gfx_opengl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/graphic/Fast3D/gfx_sdl.h
        ${CMAKE_CURRENT_SOURCE_DIR}/graphic/Fast3D/gfx_sdl2.cpp
    )
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND Source_Files__Graphic
        ${CMAKE_CURRENT_SOURCE_DIR}/graphic/Fast3D/gfx_dxgi.h
        ${CMAKE_CURRENT_SOURCE_DIR}/graphic/Fast3D/gfx_dxgi.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/graphic/Fast3D/gfx_direct3d11.h
        ${CMAKE_CURRENT_SOURCE_DIR}/graphic/Fast3D/gfx_direct3d11.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/graphic/Fast3D/gfx_direct3d12.h
        ${CMAKE_CURRENT_SOURCE_DIR}/graphic/Fast3D/gfx_direct3d12.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/graphic/Fast3D/gfx_direct3d_common.h
        ${CMAKE_CURRENT_SOURCE_DIR}/graphic/Fast3D/gfx_direct3d_common.cpp
    )
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    list(APPEND Source_Files__Graphic
        ${CMAKE_CURRENT_SOURCE_DIR}/graphic/Fast3D/gfx_metal.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/graphic/Fast3D/gfx_metal.h
        ${CMAKE_CURRENT_SOURCE_DIR}/graphic/Fast3D/gfx_metal_shader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/graphic/Fast3D/gfx_metal_shader.h
    )
elseif (CMAKE_SYSTEM_NAME STREQUAL "CafeOS")
    list(APPEND Source_Files__Graphic
        ${CMAKE_CURRENT_SOURCE_DIR}/graphic/Fast3D/gfx_wiiu.h
        ${CMAKE_CURRENT_SOURCE_DIR}/graphic/Fast3D/gfx_wiiu.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/graphic/Fast3D/gfx_gx2.h
        ${CMAKE_CURRENT_SOURCE_DIR}/graphic/Fast3D/gfx_gx2.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/graphic/Fast3D/gx2_shader_gen.h
        ${CMAKE_CURRENT_SOURCE_DIR}/graphic/Fast3D/gx2_shader_gen.c
    )
endif()

source_group("graphic" FILES ${Source_Files__Graphic})
target_sources(libultraship PRIVATE ${Source_Files__Graphic})

#=================== STB ===================

set(Source_Files__Lib__stb
    "${CMAKE_CURRENT_SOURCE_DIR}/../extern/stb/stb_image.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/../extern/stb/stb_image_write.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/../extern/stb/stb_impl.c"
)
source_group("extern/stb" FILES ${Source_Files__Lib__stb})
target_sources(libultraship PRIVATE ${Source_Files__Lib__stb})

#=================== metal-cpp ===================

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_include_directories(libultraship PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../extern/metal-cpp)
endif()


#=================== Packages & Includes ===================

target_include_directories(libultraship
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../extern ${CMAKE_CURRENT_BINARY_DIR}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../extern/spdlog/include ${CMAKE_CURRENT_SOURCE_DIR}/../extern/stb
)

if (CMAKE_SYSTEM_NAME STREQUAL "CafeOS")
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${DEVKITPRO}/portlibs/wiiu/include/
    )
endif()

#=================== Linking ===================

target_link_libraries(libultraship
    PRIVATE StrHash64
    PUBLIC ZAPDUtils ImGui storm tinyxml2 nlohmann_json::nlohmann_json GLESv3#GL
)

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin" OR CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(libultraship PRIVATE Threads::Threads)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    find_Library(OSX_FOUNDATION Foundation)
    find_Library(OSX_AVFOUNDATION AVFoundation)
    find_library(METAL Metal)
    find_library(QUARTZCORE QuartzCore)
    target_link_libraries(libultraship PRIVATE ${OSX_FOUNDATION} ${OSX_AVFOUNDATION} ${METAL} ${QUARTZCORE} ${CMAKE_DL_LIBS})
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(PulseAudio)
    target_link_libraries(libultraship PRIVATE ${PULSEAUDIO_LIBRARY})
endif()

#=================== Compile Options & Defs ===================

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        WIN32
        _CONSOLE
        _CRT_SECURE_NO_WARNINGS
        ENABLE_DX11
    )
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES 
    )
endif()

if (NOT CMAKE_SYSTEM_NAME STREQUAL "CafeOS")
    target_compile_definitions(libultraship PRIVATE
        ENABLE_OPENGL
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
        $<$<CONFIG:Debug>:SPDLOG_ACTIVE_LEVEL=0>
        $<$<NOT:$<CONFIG:Debug>>:SPDLOG_ACTIVE_LEVEL=1>
    )
else ()
    target_compile_definitions(libultraship PRIVATE
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
        SPDLOG_NO_THREAD_ID
        SPDLOG_NO_TLS
        STBI_NO_THREAD_LOCALS
        SPDLOG_ACTIVE_LEVEL=3
    )
endif()

if(MSVC)
    target_compile_options(libultraship PRIVATE
        $<$<CONFIG:Debug>:
            /Od;
            /Oi-
        >
        $<$<CONFIG:Release>:
            /Oi;
            /Gy
        >
        /permissive-;
        /MP;
        /sdl;
        /W3;
        ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
        ${DEFAULT_CXX_EXCEPTION_HANDLING};
    )
    target_link_options(libultraship PRIVATE
        $<$<CONFIG:Release>:
            /OPT:REF;
            /OPT:ICF
        >
        /SUBSYSTEM:CONSOLE
    )
endif()

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU|Clang")
	target_compile_options(libultraship PRIVATE
		-Wall
		-Wextra
		-Wno-error
		-Wno-unused-variable
		-Wno-unused-parameter
		-Wno-unused-function
		-Wno-parentheses
		-Wno-narrowing
		-Wno-missing-field-initializers
        $<$<COMPILE_LANGUAGE:C,OBJC>:-Wno-int-conversion>
	)
endif()
