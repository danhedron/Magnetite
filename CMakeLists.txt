### Open Craft CMake build scripts ~
# 
# Written By Daniel `Haribo' Evans
#  Licenced under the X11 License.

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

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
#Find SFML
find_package(SFML REQUIRED)
if(SFML_FOUND)
	target_link_libraries(OpenCraft ${SFML_LIBRARY} OpenGl32.lib glu32.lib winmm.lib)
	include_directories(${SFML_INCLUDE_DIR})
endif(SFML_FOUND)

# Bin = ${OPEN_BINARY_DIR}
