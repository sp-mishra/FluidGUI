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

# Define the source and destination directories
set(WEB_SOURCE_DIR "${CMAKE_SOURCE_DIR}/web")
set(BIN_DEST_DIR "${CMAKE_BINARY_DIR}/bin")

# Copy the web directory to the bin directory
file(COPY ${WEB_SOURCE_DIR} DESTINATION ${BIN_DEST_DIR})

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
set(SPDLOG_FMT_EXTERNAL_HO OFF)

# Graaf
set(SKIP_TESTS ON)
set(SKIP_EXAMPLES ON)
set(SKIP_BENCHMARKS ON)

# lexbor
set(LEXBOR_BUILD_SHARED OFF)
set(LEXBOR_BUILD_STATIC ON)

# inja
set(INJA_BUILD_TESTS OFF)
set(BUILD_TESTING OFF)
set(INJA_EXPORT OFF)
set(INJA_USE_EMBEDDED_JSON ON)

# reflect-cpp
set(REFLECTCPP_JSON ON)
set(REFLECTCPP_BSON OFF)
set(REFLECTCPP_CBOR OFF)
set(REFLECTCPP_XML OFF)

include(FetchContent)
include_directories(include)
include_directories(third_party)

FetchContent_Declare(
        webview
        GIT_REPOSITORY https://github.com/webview/webview
        GIT_TAG 0.12.0)
FetchContent_MakeAvailable(webview)

# Add Duktape
set(DUKTAPE_SRC_DIR "${CMAKE_SOURCE_DIR}/third_party/duktape")
set(DUKTAPE_SRC_FILES
    ${DUKTAPE_SRC_DIR}/duk_config.h
    ${DUKTAPE_SRC_DIR}/duktape.c
    ${DUKTAPE_SRC_DIR}/duktape.h
    include/JavaScript.hpp
    )
add_library(duktape STATIC ${DUKTAPE_SRC_FILES})
set_target_properties(duktape PROPERTIES LINKER_LANGUAGE C)
target_include_directories(duktape PUBLIC ${DUKTAPE_SRC_DIR})

#add_subdirectory(third_party/Catch2)
add_subdirectory(third_party/spdlog)
add_subdirectory(third_party/reflect-cpp)
add_subdirectory(third_party/csv-parser)
add_subdirectory(third_party/graaf)
add_subdirectory(third_party/lexbor)
#add_subdirectory(third_party/maddy)
add_subdirectory(third_party/inja)
add_subdirectory(third_party/eventpp)

find_library(COREGRAPHICS_LIBRARY CoreGraphics)
add_executable(${PROJECT_NAME}
               include/Log.hpp
               include/W2UIHtmlGenerator.hpp
               main.cpp
               include/UIDom.hpp
               include/ScreenUtils.hpp
               include/FileUtils.hpp
               include/HtmlUtility.hpp
               include/WidgetEdsl.hpp
               include/Components.hpp
               include/StringUtils.hpp
               include/ComponentConstants.hpp
               )
#target_sources(${PROJECT_NAME} PRIVATE main.cpp
#
#)
target_link_libraries(${PROJECT_NAME} PRIVATE
                      webview::core
                      #        Catch2::Catch2
                      eventpp::eventpp
                      spdlog::spdlog
                      inja
                      reflectcpp
                      Graaf::Graaf
                      csv
                      lexbor_static
                      #        maddy
                      duktape
                      ${COREGRAPHICS_LIBRARY}
                      )