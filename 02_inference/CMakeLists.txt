cmake_minimum_required(VERSION 3.14)

project(qnn 
    VERSION 1.0 
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    message(STATUS "Building in debug mode")
    add_compile_definitions(BUILD_DEBUG)
elseif(CMAKE_BUILD_TYPE MATCHES "Release")
    add_compile_definitions(BUILD_RELEASE)
endif()

include(FetchContent)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(fetch/fmt)
include(fetch/nlohmann)
include(fetch/spdlog)
include(fetch/stb)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_subdirectory(include)
add_subdirectory(tutorials)