cmake_minimum_required(VERSION 3.18)

project(FluidGUI LANGUAGES CXX C)
set(PROJECT_NAME FluidGUI)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # optional, ensure standard is supported
set(CMAKE_CXX_EXTENSIONS OFF)  # optional, keep compiler extensions off
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(COPYRIGHT "Copyright 2024, GrokkingLab")
set(VENDOR "GrokkingLab")
set(DESCRIPTION "C++ GUI Using web technologies")


find_package(Threads REQUIRED)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(WEBVIEW_BUILD_SHARED_LIBRARY OFF)
set(WEBVIEW_BUILD_STATIC_LIBRARY ON)
set(WEBVIEW_BUILD_SHARED OFF)
set(BUILD_STATIC ON)

# Spdlog
set(SPDLOG_BUILD_EXAMPLES OFF)
set(SPDLOG_USE_STD_FORMAT ON)
set(SPDLOG_BUILD_SHARED OFF)
set(SPDLOG_ENABLE_PCH ON)
set(SPDLOG_BUILD_EXAMPLE OFF)
set(SPDLOG_BUILD_TESTS OFF)
set(SPDLOG_BUILD_BENCH OFF)

# Graaf
set(SKIP_TESTS OFF)
set(SKIP_EXAMPLES OFF)
set(SKIP_BENCHMARKS OFF)

# lexbor
set(LEXBOR_BUILD_SHARED OFF)
set(LEXBOR_BUILD_STATIC ON)

include(FetchContent)
include_directories(include)

FetchContent_Declare(
        webview
        GIT_REPOSITORY https://github.com/webview/webview
        GIT_TAG 0.12.0)
FetchContent_MakeAvailable(webview)

#add_subdirectory(third_party/Catch2)
add_subdirectory(third_party/spdlog)
add_subdirectory(third_party/reflect-cpp)
add_subdirectory(third_party/json)
add_subdirectory(third_party/csv-parser)
add_subdirectory(third_party/graaf)
add_subdirectory(third_party/lexbor)

find_library(COREGRAPHICS_LIBRARY CoreGraphics)
add_executable(${PROJECT_NAME}
        include/Log.hpp
        include/HtmlGenerator.hpp
        main.cpp
)
#target_sources(${PROJECT_NAME} PRIVATE main.cpp
#
#)
target_link_libraries(${PROJECT_NAME} PRIVATE
        webview::core
#        Catch2::Catch2
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        reflectcpp
        Graaf::Graaf
        csv
        lexbor_static
        ${COREGRAPHICS_LIBRARY}
)