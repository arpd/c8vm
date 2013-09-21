cmake_minimum_required (VERSION 2.6)
project (c8vm)

# look for scripts in ./cmake/Modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set (PROJECT_SOURCE_DIR src/)
set (PROJECT_BINARY_DIR bin/)
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
# ver
set (c8vm_VERSION_MAJOR 0)
set (c8vm_VERSION_MINOR 1)

# compiler config
set (CMAKE_CC_COMPILER  "/usr/bin/clang")
set (CMAKE_CXX_COMPILER "/usr/bin/clang++")
set (CMAKE_CXX_FLAGS "-g --std=c++11 -Wall -DDEBUG")

# configure header file to pass some cmake settings to the src
configure_file (
    "${PROJECT_SOURCE_DIR}/c8_config.h.cmake"
    "${PROJECT_SOURCE_DIR}/c8_config.h"
)

# opengl stuff
INCLUDE(FindOpenGL REQUIRED)
INCLUDE(FindGLEW REQUIRED)
INCLUDE(FindGLUT REQUIRED)
INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${GLEW_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${GLUT_INCLUDE_DIR})

message(STATUS "OPENGL_LIBRARIES: " ${OPENGL_LIBRARIES})
message(STATUS "  GLUT_LIBRARIES: " ${GLUT_LIBRARIES})
message(STATUS "GLUT_INCLUDE_DIR: " ${GLUT_INCLUDE_DIR})
message(STATUS "  GLEW_LIBRARIES: " ${GLEW_LIBRARIES})
message(STATUS "GLEW_INCLUDE_DIR: " ${GLEW_INCLUDE_DIR})

# add the binary tree to the search path for include files so that we will find
# c8.h
include_directories("${PROJECT_BINARY_DIR}")

add_executable (
        c8vm
        ${PROJECT_SOURCE_DIR}/c8vm.cpp
        ${PROJECT_SOURCE_DIR}/c8.cpp
        ${PROJECT_SOURCE_DIR}/iset.cpp
)

target_link_libraries(c8vm ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
