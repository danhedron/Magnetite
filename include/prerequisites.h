#define NOMINMAX
#include <GL/GLee.h>
#include <GL/freeglut.h>
#include <SFML/Window.hpp>
#include <SFML/Window/OpenGL.hpp>
#include <SFML/Graphics.hpp>

#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <map>
#include <limits>
#include <iostream>

#include "util.h"
#include "Vector.h"
#include "Matrix.h"

typedef std::vector<std::string> stringvector;

class BaseBlock;
typedef std::map<size_t,BaseBlock*> BlockList;
typedef std::pair<const size_t,BaseBlock*> BlockPosPair;
//typedef *BaseBlock[] BlockList;

class WorldChunk;
typedef std::vector<WorldChunk*> ChunkList;

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 128
#define CHUNK_SIZE CHUNK_WIDTH*CHUNK_WIDTH*CHUNK_HEIGHT
