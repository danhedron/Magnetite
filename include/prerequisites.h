#ifndef _PREREQUISITES_H_
#define _PREREQUISITES_H_

/**
 * Libaries
 */
//#define NOMINMAX
#include <limits.h>
#include <GL/GLee.h>
#undef None
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <btBulletDynamicsCommon.h>

#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <map>
#include <limits>
#include <iostream>
#include <stdio.h>

#include "Vector.h"
#include "Matrix.h"

typedef long ChunkScalar;

/**
 * @struct ChunkIndex
 * contains the 3 components of a chunk's index in the world
 */
struct ChunkIndex
{
	ChunkScalar x;
	ChunkScalar y;
	ChunkScalar z;
};

/**
 * Define vector type
 */
typedef std::vector<std::string> stringvector;

class BaseBlock;
typedef BaseBlock* BlockPtr;
typedef BlockPtr* BlockArray;
typedef std::map<size_t,BaseBlock*> BlockList;
typedef std::pair<const size_t,BaseBlock*> BlockPosPair;
typedef unsigned char LightIndex;

//typedef *BaseBlock[] BlockList;

namespace Magnetite {
	class Component;
	typedef std::vector<Component*> ComponentList;
	
	class BaseEntity;
	typedef std::vector<BaseEntity*> EntityList;
	
	typedef size_t EntityID;
	
	typedef std::string String;
};

class Geometry;
typedef Geometry* GeometryPtr;

class Chunk;
//typedef std::vector<WorldChunk*> ChunkList;

#define CoreSingleton (MagnetiteCore::Singleton)

#define CHUNK_WIDTH 32
#define CHUNK_HEIGHT CHUNK_WIDTH
#define CHUNK_SIZE (CHUNK_WIDTH*CHUNK_WIDTH*CHUNK_HEIGHT)
#define NODE_SIZE ((CHUNK_HEIGHT/CHUNK_WIDTH))

#define REGION_SIZE 8
#define REGION_WORLD_SIZE (REGION_SIZE * CHUNK_WIDTH)

//#define BLOCK_POSITION( index ) index % CHUNK_WIDTH * CHUNK_HEIGHT , (index - std::floorf( index / CHUNK_WIDTH*CHUNK_HEIGHT ) - (index % CHUNK_WIDTH))   , std::floorf( index / CHUNK_WIDTH*CHUNK_HEIGHT ) 
#define BLOCK_INDEX_2( x, y, z ) ( z * CHUNK_WIDTH * CHUNK_HEIGHT + y * CHUNK_WIDTH + x )
#define BLOCK_INDEX( block ) (block->getZ() * CHUNK_WIDTH * CHUNK_HEIGHT + block->getY() * CHUNK_WIDTH + block->getX())

#define PRINT_GLERROR {GLenum err = glGetError(); if(err) Util::log(Util::toString(err));}

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

#include "util.h"

#endif