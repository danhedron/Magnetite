#include <glee.h>
#include <SFML/Window.hpp>
#include <SFML/Window/OpenGL.hpp>

#include <vector>
#include <map>
#include <iostream>

class BaseBlock;
typedef std::map<size_t,BaseBlock*> BlockList;
typedef std::pair<size_t,BaseBlock*> BlockPosPair;
//typedef *BaseBlock[] BlockList;

#define CHUNK_WIDTH 32
#define CHUNK_HEIGHT 128
#define CHUNK_SIZE CHUNK_WIDTH*CHUNK_WIDTH*CHUNK_HEIGHT