cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard to use")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 ")

project(GAME)

set(ANDROID_NATIVE_API_LEVEL 23) # Adjust to your desired API level # or other ABI like arm64-v8a, x86, etc.

# armeabi-v7a requires cpufeatures library
# include(AndroidNdkModules)
# android_ndk_import_module_cpufeatures()


# SDL sources are in a subfolder named "SDL"
#add_subdirectory(SDL2)

#include_directories(${CMAKE_CURRENT_BINARY_DIR}/glew)

#add_subdirectory(gl4es)

# generated files from libpng during build
include_directories(${CMAKE_CURRENT_BINARY_DIR}/libpng)
add_subdirectory(libpng)

#add_subdirectory(glad)

# Compilation of companion libraries
#add_subdirectory(SDL_image)
#add_subdirectory(SDL_mixer)
#add_subdirectory(SDL_ttf)

# Your game and its CMakeLists.txt are in a subfolder named "src"
add_subdirectory(src)

