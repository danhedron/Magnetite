#ifndef _RENDERER_H_
#define _RENDERER_H_
#include "prerequisites.h"

class WorldChunk;
class BaseBlock;
class Camera;

/**
 * @struct Struct to represent a vertex, memory aligned for easy array-xing
 */
struct GLvertex {
	float x, y, z;
	float nx, ny, nz;
	float u0, v0;
};
typedef unsigned short GLedge;

struct GLgeometry {
	GLedge* edgeData;
	GLvertex* vertexData;
	size_t edgeCount;
	size_t vertexCount;
	~GLgeometry() { delete[] edgeData; delete[] vertexData; }
};
struct GLbuffer {
	GLuint vertex;
	GLuint index;
};

//typedef std::map<WorldChunk*,GLeometry> ChunkGeomList;
//typedef std::map<WorldChunk*,GLbuffer> ChunkGeomList;

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

	Camera* mCamera;

	size_t mFpsAvg;
	
	//ChunkGeomList mWorldBuffers;

	static GLvertex vertex( float x, float y, float z, float nx, float ny, float nz, float u = 0.f, float v = 0.f, float w = 0.f );

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

	/**
	 * Sets up the OpenGL context for 2D Rendering
	 */
	void enable2D();

	/**
	 * Sets OpenGL back up for 3D rendering.
	 */
	void disable2D();

	/**
	 * Sets the active rendering camera
	 * @param cam Pointer to camera
	 */
	void setCamera( Camera* cam );
	
	Camera* getCamera();

	void render( double dt, std::vector<WorldChunk*> &chunks );

	/**
	 * Draws debug statistics onto the screen
	 */
	void drawStats( double dt, size_t chunkCount );

	/**
	 * Draws the block chooser
	 */
	void drawBlockChooser( double dt );

	/**
	 * Chooses the next block 
	 */
	void nextBlock();
	void lastBlock();
	std::string blockType;

	/**
	 * Draws the string on the screen (coords start from top left)
	 * Will split each line into a new draw call
	 * @param text Text to draw
	 * @param x Coordinate 
	 * @param y Coordinate
	 */
	void drawText( std::string text, int x, int y );

	static void buildCubeData(BaseBlock* block, size_t& ind, size_t& eInd, GLvertex* data, GLedge* edges);

	/**
	 * Notify the renderer that a chunk has been unloaded from the engine and it is ok to release it's graphics buffer.
	 * @param chunk pointer to the unloaded chunk
	 */
	void notifyChunkUnloaded( WorldChunk* chunk );

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