#ifndef _RENDERER_H_
#define _RENDERER_H_
#include "prerequisites.h"

class WorldChunk;
class BaseBlock;

class Renderer
{
protected:
	float totalTime;
	GLuint chunkVbo;
	GLuint chunkSize;

	size_t mScrWidth;
	size_t mScrHeight;

	size_t mBlRendered;
	size_t mBlTotal;
public:
	Renderer(void);
	~Renderer(void);

	void initialize(int *argc, char **argv);
	void resizeViewport( size_t x, size_t y, size_t w, size_t h );

	void enable2D();
	void disable2D();

	void render( double dt, std::vector<WorldChunk*> &chunks );

	void buildChunkVBO(WorldChunk* chunk);
	void buildCubeData(BaseBlock* block, size_t& ind, GLfloat* data);

};

#endif