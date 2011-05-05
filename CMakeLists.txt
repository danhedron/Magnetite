### Open Craft CMake build scripts ~
#
# Written By Daniel `Haribo' Evans
# Copyright (C) 2011 Daniel Lloyd Evans
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

cmake_minimum_required(VERSION 2.6)
project(OpenCraft)
#Versions
set (OPENCRAFT_VERSION_MAJOR 0)
set (OPENCRAFT_VERSION_MINOR 1)
#Pass important things to Code
configure_file(
	"${PROJECT_SOURCE_DIR}/include/BuildConfig.h.in"
	"${PROJECT_SOURCE_DIR}/include/BuildConfig.h"
)

set(OPENCRAFT_SOURCES
	source/OpencraftCore.cpp
	source/Main.cpp
	source/Renderer.cpp
)
set(OPENCRAFT_HEADERS
	include/OpencraftCore.h
	include/prerequisites.h
	include/BuildConfig.h
	include/Renderer.h
)
source_group("Header Files" FILES ${OPENCRAFT_HEADERS})
source_group("Source Files" FILES ${OPENCRAFT_SOURCES})

add_executable(OpenCraft ${OPENCRAFT_SOURCES} ${OPENCRAFT_HEADERS})

include_directories(${PROJECT_SOURCE_DIR}/include)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")
#Find SFML
find_package(SFML REQUIRED)
if(SFML_FOUND)
	target_link_libraries(OpenCraft ${SFML_LIBRARY})
	include_directories(${SFML_INCLUDE_DIR})
endif(SFML_FOUND)

#Find OpenGl
find_package(OpenGL REQUIRED)
if(OpenGL_FOUND)
	target_link_libraries(OpenCraft ${OpenGL_LIBRARY})
	include_directories(${OpenGL_INCLUDE_DIR})
endif(OpenGL_FOUND)

# Bin = ${OPEN_BINARY_DIR}
