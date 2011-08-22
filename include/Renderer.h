#ifndef _RENDERER_H_
#define _RENDERER_H_
#include "prerequisites.h"

class WorldChunk;
class BaseBlock;
class Camera;
class World;

/**
 * @struct Struct to represent a vertex, memory aligned for easy array-xing
 */
struct GLvertex {
	float x, y, z;
	/*float nx, ny, nz;*/ // Normals are undeeded at the moment.
	float u0, v0;
	float r, g, b;
};
typedef unsigned short GLedge;

struct GLgeometry {
	GLedge* edgeData;
	GLvertex* vertexData;
	size_t edgeCount;
	size_t vertexCount;
	GLgeometry() { vertexBO = 0; indexBO = 0; }
	~GLgeometry() { releaseBuffer(); delete[] edgeData; delete[] vertexData; }
	GLuint vertexBO;
	GLuint indexBO;
	void releaseBuffer();
	void bindToBuffer();
};
struct GLbuffer {
	GLuint vertex;
	GLuint index;
};

struct GLshader {
	GLuint ref;
	GLenum type;
	std::string source;
	std::string filename;

	GLshader() { ref = 0; type = 0; }
	void create();
};

struct GLprogram {
	GLuint ref;
	GLshader* vertex;
	GLshader* fragment;
	std::map<std::string, GLuint> mUniforms;

	GLprogram() { ref = 0; vertex = NULL; fragment = NULL; }
	void link();
	void bindUniformTexture( std::string var, GLint unit );
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
	size_t mDebugMode;

	Camera* mCamera;

	size_t mFpsAvg;

	bool mDrawFrustum;
	bool mDrawWorld;
	
	sf::RenderWindow* mWindow;

	//ChunkGeomList mWorldBuffers;

	std::map<std::string, GLshader*> mShaders;

	GLprogram mWorldProgram;
	GLprogram mLightingProgram;

public:
	Renderer(void);
	~Renderer(void);

	static GLvertex vertex( float x, float y, float z, float nx = 0.f, float ny = 0.f, float nz = 0.f, float u = 0.f, float v = 0.f, float r = 0.f, float g = 0.f, float b = 0.f );

	/**
	 * Rendermode enum
	 */
	enum {
		RENDER_SOLID,
		RENDER_LIGHTING
	};

	/**
	 * Debug options
	 */
	enum {
		DEBUG_OFF,
		DEBUG_STATS,
		DEBUG_PERF,
		DEBUG_TERRAIN,
		DEBUG_PHYSICS
	};

	/**
	 * Initializes graphics & creates a window.
	 * @param argc The number of arguments passed.
	 * @param argv char pointer array
	 */
	void initialize(sf::RenderWindow& window);

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

	/**
	 * Loads a named shader from a file
	 */
	GLshader* loadShader( std::string filename, GLenum type );

	/**
	 * Unloads a named shader
	 */
	void unloadShader( std::string filename );

	void render( double dt, World* world );

	/**
	 * Finds out if the given node is in the view frustum, and if so continues to children or draws the chunk on the node
	 */
	void _renderNode( WorldNode* node, int depth  );
	void _renderChunk( WorldChunk* chunk );

	/**
	 * Draws debug statistics onto the screen
	 */
	void drawStats( double dt, size_t chunkCount, World* world );

	/**
	 * Draws the block chooser
	 */
	void drawBlockChooser( double dt );

	/**
	 * Draws the crosshair
	 */
	void drawCrosshair( double dt );

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
	 * Toggles the drawing of the camera's frustum
	 */
	void toggleCameraFrustum();

	/**
	 * Sets wether or not to draw the world
	 */
	void setWorldVisible( bool vis );

	bool isWorldVisible();

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

	/**
	 * Sets the Debug mode
	 */
	void setDebugMode( size_t debugMode );
};

#endif