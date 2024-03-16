cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard to use")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 ")

project(Game VERSION 8.0.5 LANGUAGES C CXX)
set(PROJECT_BUILD_NAME "MacReady Foxtrot" CACHE STRING "")
set(PROJECT_TEAM "github.com/harbourmasters" CACHE STRING "")

add_subdirectory(src)

