cmake_minimum_required(VERSION 3.9.2)
project(libs LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()
message(STATUS "Build lib in ${CMAKE_BUILD_TYPE} mode")
if (CMAKE_BUILD_TYPE STREQUAL Debug)
    message(STATUS "Build in debug mode, enable fiu")
    add_definitions(-D FIU_ENABLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic")
endif()

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -lpthread -lm -fPIC -ggdb3 -lunwind -fno-omit-frame-pointer -Wno-unused-function ${CMAKE_CXX_FLAGS}")
if(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "x86_64")
    set(CMAKE_CXX_FLAGS "-Werror ${CMAKE_CXX_FLAGS}")
endif()

find_package(Git)
execute_process(COMMAND ${GIT_EXECUTABLE} log -1 --format=%H
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

string(TIMESTAMP BUILD_TIME "%Y-%m-%d %H:%M")
add_definitions("-DGIT_SHA1=${GIT_HASH}")
configure_file("${CMAKE_SOURCE_DIR}/include/lib/version.h.in" "${CMAKE_BINARY_DIR}/include/lib/version.h" @ONLY)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/modules/")
include(ReadVersion)
read_version(lib_VERSION)
set(lib_VERSION ${lib_VERSION})

option(ENABLE_GCOV "enable gcov for coverate test" OFF)
if (ENABLE_GCOV)
    message(STATUS "enable coverage analysis with gcov")
    set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE ON)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif()

option(ENABLE_ASAN "enable address sanitizer" OFF)
if (ENABLE_ASAN)
    message(STATUS "enable address sanitizer")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    add_compile_options("-fsanitize=address")
    link_libraries("-fsanitize=address")
endif()

add_subdirectory(third)
enable_testing()
include(AddTest)
add_subdirectory(lib)
