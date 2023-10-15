#find_package(OpenGL QUIET)
find_library(OPENGL_LIBRARY GLESv2)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    find_package(SDL2 CONFIG REQUIRED)
else()
    #find_package(SDL2 REQUIRED)
endif()

#=================== DrLibs ===================

set(DRLIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dr_libs)
add_library(DrLibs INTERFACE)

target_include_directories(DrLibs INTERFACE ${DRLIBS_DIR})

#=================== Storm ===================

set(STORMLIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/StormLib)
add_subdirectory(${STORMLIB_DIR})
if((CMAKE_SYSTEM_NAME MATCHES "Windows") AND ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"))
    include(../cmake/HandleCompilerRT.cmake)
    find_compiler_rt_library(builtins CLANG_RT_BUILTINS_LIBRARY)
    get_filename_component(LIBDIR "${CLANG_RT_BUILTINS_LIBRARY}" DIRECTORY)
    if(IS_DIRECTORY "${LIBDIR}")
        target_link_libraries(storm ${CLANG_RT_BUILTINS_LIBRARY})
    endif()
endif()
if (CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
    target_compile_definitions(storm PRIVATE -D_POSIX_C_SOURCE=200809L)
endif()

#=================== ImGui ===================

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ImGui)
add_library(ImGui STATIC)
set_property(TARGET ImGui PROPERTY CXX_STANDARD 20)

target_sources(ImGui
    PRIVATE
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui.cpp
)

if (CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
    target_include_directories(ImGui PRIVATE ${DEVKITPRO}/portlibs/switch/include/)
endif()

if (NOT CMAKE_SYSTEM_NAME STREQUAL "CafeOS")
    target_sources(ImGui
        PRIVATE
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
        ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
    )
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_sources(ImGui
        PRIVATE
        ${IMGUI_DIR}/backends/imgui_impl_dx11.cpp
        ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
    )
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_sources(ImGui
        PRIVATE
        ${IMGUI_DIR}/backends/imgui_impl_metal.mm
    )
elseif (CMAKE_SYSTEM_NAME STREQUAL "CafeOS")
    target_sources(ImGui
        PRIVATE
        ${IMGUI_DIR}/backends/wiiu/imgui_impl_wiiu.cpp
        ${IMGUI_DIR}/backends/wiiu/imgui_impl_gx2.cpp
    )
endif()

target_include_directories(ImGui PUBLIC ${IMGUI_DIR} ${IMGUI_DIR}/backends PRIVATE ${SDL2_INCLUDE_DIRS})

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_include_directories(ImGui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/metal-cpp)
    target_compile_definitions(ImGui PUBLIC IMGUI_IMPL_METAL_CPP)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(ImGui PUBLIC SDL2::SDL2 SDL2::SDL2main)
elseif (CMAKE_SYSTEM_NAME STREQUAL "CafeOS")
    target_link_libraries(ImGui PUBLIC SDL2::SDL2-static)
else()
    target_link_libraries(ImGui PUBLIC SDL2::SDL2)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    find_package(GLEW REQUIRED)
    target_link_libraries(ImGui PUBLIC opengl32 GLEW::GLEW)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    find_package(GLEW REQUIRED)
    target_link_libraries(ImGui PUBLIC ${OPENGL_opengl_LIBRARY} GLEW::GLEW)
    set_target_properties(ImGui PROPERTIES
        XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES 
    )
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(GLEW REQUIRED)
    target_link_libraries(ImGui PUBLIC ${OPENGL_glx_LIBRARY} ${OPENGL_opengl_LIBRARY} GLEW::GLEW)
endif()

#=================== tinyxml2 ===================

set(TINYXML2_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tinyxml2)
add_library(tinyxml2 STATIC)

target_sources(tinyxml2 PRIVATE ${TINYXML2_DIR}/tinyxml2.cpp)
target_include_directories(tinyxml2 PUBLIC ${TINYXML2_DIR})

#=================== ZAPDUtils ===================

set(ZAPDUTILS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ZAPDUtils)
add_subdirectory(${ZAPDUTILS_DIR})

#=================== nlohmann-json ===================

set(JSON_BuildTests OFF CACHE INTERNAL "")

set(NLOHMANN_JSON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/nlohmann-json)
add_subdirectory(${NLOHMANN_JSON_DIR})

#=================== StrHash64 ===================

set(STRHASH64_DIR ${CMAKE_CURRENT_SOURCE_DIR}/StrHash64)
add_library(StrHash64 STATIC)

target_sources(StrHash64 PRIVATE ${STRHASH64_DIR}/StrHash64.cpp)

target_include_directories(StrHash64 PUBLIC ${STRHASH64_DIR})

#=================== thread-pool ===================

set(THREADPOOL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thread-pool)
add_library(ThreadPool INTERFACE)

target_include_directories(ThreadPool INTERFACE ${THREADPOOL_DIR})
