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

	size_t mRenderMode;
public:
	Renderer(void);
	~Renderer(void);

	/**
	 * Rendermode enum
	 */
	enum {
		RENDER_SOLID,
		RENDER_WIRE
	};

	/**
	 * Initializes graphics & creates a window.
	 * @param argc The number of arguments passed.
	 * @param argv char pointer array
	 */
	void initialize(int *argc, char **argv);

	/**
	 * Sets up OGL for the specifed viewport.
	 * @param x X
	 * @param y Y
	 * @param w Width
	 * @param h Height
	 */
	void resizeViewport( size_t x, size_t y, size_t w, size_t h );

	void enable2D();
	void disable2D();

	void render( double dt, std::vector<WorldChunk*> &chunks );

	void drawStats( double dt, size_t chunkCount );

	/**
	 * Draws the string on the screen (coords start from top left)
	 * Will split each line into a new draw call
	 * @param text Text to draw
	 * @param x Coordinate 
	 * @param y Coordinate
	 */
	void drawText( std::string text, int x, int y );

	void buildChunkVBO(WorldChunk* chunk);
	void buildCubeData(BaseBlock* block, size_t& ind, GLfloat* data);

	/**
	 * Sets the internal rendermode:-
	 * * RENDER_SOLID: Render with textures.
	 * * RENDER_WIRE: Render in wireframe.
	 * @param rendermode Rendermode to switch to.
	 */
	void setRenderMode( size_t rendermode );

	size_t getRenderMode();

};

#endif