#include <SFML/Window.hpp>
#include <SFML/Window/OpenGL.hpp>

#include <vector>
#include <iostream>

class BaseBlock;
typedef std::vector<BaseBlock*> BlockList;

#define CHUNK_WIDTH 32
#define CHUNK_HEIGHT 128
#define CHUNK_SIZE CHUNK_WIDTH*CHUNK_WIDTH*CHUNK_HEIGHT