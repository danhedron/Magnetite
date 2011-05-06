#ifndef _RENDERER_H_
#define _RENDERER_H_
#include "prerequisites.h"

class WorldChunk;
class BaseBlock;

class Renderer
{
protected:
	float totalTime;
public:
	Renderer(void);
	~Renderer(void);

	void initialize();
	void resizeViewport( size_t x, size_t y, size_t w, size_t h );

	void render( double dt, std::vector<WorldChunk*> &chunks );

	void buildChunkVBO(WorldChunk* chunk);
	void buildCubeData(BaseBlock* block, size_t& ind, GLfloat* data);

};

#endif