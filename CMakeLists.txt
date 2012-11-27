### Magnetite CMake build scripts ~
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
project(Magnetite)
#Versions
set (MAGNETITE_VERSION_MAJOR 0)
set (MAGNETITE_VERSION_MINOR 1)

# Set GCC flags to some sensible defaults
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -O")

#Pass important things to Code
configure_file(
	"${PROJECT_SOURCE_DIR}/include/BuildConfig.h.in"
	"${PROJECT_SOURCE_DIR}/include/BuildConfig.h"
)

set(MAGNETITE_SOURCES
	source/MagnetiteCore.cpp
	source/Main.cpp
	source/Tests.cpp
	source/Chunk.cpp
	source/BaseGame.cpp
	source/ScriptGame.cpp
	source/Collision.cpp
	source/Renderer.cpp
	source/renderer/ProgramResource.cpp
	source/renderer/Geometry.cpp
	source/InputManager.cpp
	source/BulletDebug.cpp
	source/LightingManager.cpp
	source/PhysicsState.cpp
	source/ChunkPhysicsState.cpp
	source/WorldObject.cpp
	
	source/BaseEntity.cpp
	source/Component.cpp
	source/Components/InfoComponent.cpp
	source/Components/InterpolatorComponent.cpp
	source/Components/PhysicsComponent.cpp
	source/Components/RenderableComponent.cpp
	source/script/ScriptEntity.cpp
	
	# Paging 
	source/Region.cpp
	source/paging/PagingCamera.cpp
	source/paging/PagingContext.cpp
	source/WorldSerializer.cpp
	
	source/PhysicsWorldObject.cpp
	source/WorldItem.cpp
	source/BaseResource.cpp
	source/ShaderResource.cpp
	source/ResourceManager.cpp
	source/resources/ModelResource.cpp
	source/resources/Texture.cpp
	source/Sky.cpp
	source/Frustum.cpp
	source/Camera.cpp
	source/Matrix.cpp
	source/Vector.cpp
	source/Character.cpp
	source/Player.cpp
	source/BlockFactory.cpp
	source/BaseBlock.cpp
	source/StoneBlock.cpp
	source/CobbleBlock.cpp
	source/MossCobbleBlock.cpp
	source/DirtBlock.cpp
	source/GrassBlock.cpp
	source/SandBlock.cpp
	source/WoodBlock.cpp
	source/WaterBlock.cpp
	source/MovingBlock.cpp
	source/ChunkGenerator.cpp
	source/World.cpp
	source/Explosion.cpp
	source/TextureManager.cpp
	source/geometry/BaseTriangulator.cpp
	source/geometry/BlockTriangulator.cpp
	
	# Scripting Interfaces
	source/script/ScriptWrapper.cpp
	source/script/ScriptGlobalsWorld.cpp
	source/script/ScriptGlobalsDebug.cpp
	source/script/ScriptExplosion.cpp
	source/script/ScriptPlayer.cpp
	source/script/Entities.cpp
	
	source/blocks/LogBlock.cpp
	source/blocks/LeafBlock.cpp
	source/Profiler.cpp
)
set(MAGNETITE_HEADERS
	include/MagnetiteCore.h
	include/prerequisites.h
	include/Tests.h
	include/Chunk.h
	include/BaseGame.h
	include/BulletDebug.h
	include/Collision.h
	include/BuildConfig.h
	include/InputManager.h
	include/LightingManager.h
	include/Renderer.h
	include/Geometry.h
	include/PhysicsState.h
	include/ChunkPhysicsState.h
	include/WorldObject.h
	
	include/BaseEntity.h
	include/Component.h
	include/Components/InfoComponent.h
	include/Components/InterpolatorComponent.h
	include/Components/PhysicsComponent.h
	include/Components/RenderableComponent.h
	include/script/ScriptEntity.h
	
	# Paging & Serialization
	include/Region.h
	include/paging/PagingCamera.h
	include/paging/PagingContext.h
	include/WorldSerializer.h
	
	include/PhysicsWorldObject.h
	include/WorldItem.h
	include/BaseResource.h
	include/ResourceManager.h
	include/ShaderResource.h
	include/ProgramResource.h
	include/ModelResource.h
	include/Texture.h
	include/Sky.h
	include/Frustum.h
	include/Camera.h
	include/Player.h
	include/Matrix.h
	include/Vector.h
	include/Character.h
	include/BlockFactory.h
	include/StoneBlock.h
	include/CobbleBlock.h
	include/MossCobbleBlock.h
	include/DirtBlock.h
	include/GrassBlock.h
	include/SandBlock.h
	include/WoodBlock.h
	include/WaterBlock.h
	include/BaseBlock.h
	include/MovingBlock.h
	include/ChunkGenerator.h
	include/World.h
	include/Explosion.h
	include/TextureManager.h
	include/BaseTriangulator.h
	include/BlockTriangulator.h
	
	#Script Interfaces
	include/script/ScriptWrapper.h
	include/script/ScriptGlobalsList.h
	include/script/ScriptGlobalsWorld.h
	include/script/ScriptGlobalsDebug.h
	include/script/ScriptExplosion.h
	include/script/ScriptPlayer.h
	include/script/Entities.h
	
	include/blocks/LogBlock.h
	include/blocks/LeafBlock.h
	include/Profiler.h
	include/util.h
)
source_group("Header Files" FILES ${MAGNETITE_HEADERS})
source_group("Source Files" FILES ${MAGNETITE_SOURCES})

add_executable(Magnetite ${MAGNETITE_SOURCES} ${MAGNETITE_HEADERS})

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/script)
include_directories(${PROJECT_SOURCE_DIR}/include/blocks)
include_directories(${PROJECT_SOURCE_DIR}/depends/include/)

# Windows only
if(WIN32)
	# Include GLee + Freeglut
	target_link_libraries(Magnetite ${PROJECT_SOURCE_DIR}/depends/lib/GLee.lib)
	#target_link_libraries(Magnetite ${PROJECT_SOURCE_DIR}/depends/lib/freeglut.lib)
	# Windows Memory Usage
	target_link_libraries(Magnetite psapi.lib)
	# 'fix' LIBC.lib error
	set(CMAKE_EXE_LINKER_FLAGS /NODEFAULTLIB:\"LIBC.lib\")
endif(WIN32)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")
#Find SFML
find_package(SFML REQUIRED)
if(SFML_FOUND)
	target_link_libraries(Magnetite ${SFML_LIBRARY})
	include_directories(${SFML_INCLUDE_DIR})
endif(SFML_FOUND)

# Find BULLET
find_package(BULLET REQUIRED)
if(BULLET_FOUND)
	target_link_libraries(Magnetite ${BULLET_LIBRARY})
	include_directories(${BULLET_INCLUDE_DIR})
endif(BULLET_FOUND)

# Find V8
find_package(V8 REQUIRED)
if(V8_FOUND)
	target_link_libraries(Magnetite ${V8_LIBRARIES})
	include_directories(${V8_INCLUDE_DIR})
endif(V8_FOUND)

#Find OpenGl Under Linux
find_package(OpenGL REQUIRED)
if(OPENGL_FOUND)
	target_link_libraries(Magnetite ${OPENGL_LIBRARY})
	include_directories(${OPENGL_INCLUDE_DIR})
endif(OPENGL_FOUND)

find_package(GLew REQUIRED)
if(GLEW_FOUND)
	target_link_libraries(Magnetite ${GLEW_LIBRARIES})
	include_directories(${GLEW_INCLUDE_DIR})
endif(GLEW_FOUND)

# Find Assimp
find_package(ASSIMP REQUIRED)
if(ASSIMP_FOUND)
	target_link_libraries(Magnetite ${ASSIMP_LIBRARIES})
	include_directories(${ASSIMP_INCLUDE_DIR})
endif(ASSIMP_FOUND)

# Find GLM
find_package(GLM REQUIRED)
if(GLM_FOUND)
	include_directories(${GLM_INCLUDE_DIR})
endif(GLM_FOUND)


# Bin = ${OPEN_BINARY_DIR}
