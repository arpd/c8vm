cmake_minimum_required (VERSION 2.6)
project (c8vm)

set (PROJECT_SOURCE_DIR src/)
set (PROJECT_BINARY_DIR bin/)
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
# ver
set (c8vm_VERSION_MAJOR 0)
set (c8vm_VERSION_MINOR 1)

# compiler config
set (CC  "/usr/bin/clang")
set (CXX "/usr/bin/clang++")
set (CMAKE_CXX_FLAGS "-g --std=c++0x -Wall -DDEBUG")

# configure header file to pass some cmake settings to the src
configure_file (
    "${PROJECT_SOURCE_DIR}/c8_config.h.cmake"
    "${PROJECT_SOURCE_DIR}/c8_config.h"
)

# add the binary tree to the search path for include files so that we will find
# c8.h
include_directories("${PROJECT_BINARY_DIR}")

add_executable (
        c8vm
        ${PROJECT_SOURCE_DIR}/c8vm.cpp
        ${PROJECT_SOURCE_DIR}/c8.cpp
        ${PROJECT_SOURCE_DIR}/iset.cpp
)
