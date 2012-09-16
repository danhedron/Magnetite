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

#include "util.h"
#include "Vector.h"
#include "Matrix.h"

/**
 * Define vector type
 */
typedef std::vector<std::string> stringvector;

class BaseBlock;
class WorldNode;
class WorldTree;
typedef BaseBlock* BlockPtr;
typedef BlockPtr* BlockArray;
typedef std::map<size_t,BaseBlock*> BlockList;
typedef std::pair<const size_t,BaseBlock*> BlockPosPair;
typedef unsigned char LightIndex;

typedef std::vector<WorldNode*> NodeList;
typedef std::vector<WorldTree*> TreeList;

typedef long ChunkScalar;

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

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 16
#define CHUNK_SIZE (CHUNK_WIDTH*CHUNK_WIDTH*CHUNK_HEIGHT)
#define NODE_SIZE ((CHUNK_HEIGHT/CHUNK_WIDTH))

#define REGION_SIZE 8
#define REGION_WORLD_SIZE (REGION_SIZE * CHUNK_WIDTH)

#define TREE_LENGTH 512 // Defines the length of one side of an octree
#define TREE_VOLUME (TREE_LENGTH*TREE_LENGTH*TREE_LENGTH)

//#define BLOCK_POSITION( index ) index % CHUNK_WIDTH * CHUNK_HEIGHT , (index - std::floorf( index / CHUNK_WIDTH*CHUNK_HEIGHT ) - (index % CHUNK_WIDTH))   , std::floorf( index / CHUNK_WIDTH*CHUNK_HEIGHT ) 
#define BLOCK_INDEX_2( x, y, z ) ( z * CHUNK_WIDTH * CHUNK_HEIGHT + y * CHUNK_WIDTH + x )
#define BLOCK_INDEX( block ) (block->getZ() * CHUNK_WIDTH * CHUNK_HEIGHT + block->getY() * CHUNK_WIDTH + block->getX())

#define PRINT_GLERROR {GLenum err = glGetError(); if(err) Util::log(Util::toString(err));}

#define BUFFER_OFFSET(i) ((char*)NULL + (i))
