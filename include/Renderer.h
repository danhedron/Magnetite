#ifndef _RENDERER_H_
#define _RENDERER_H_
#include "prerequisites.h"

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

class BaseBlock;
class Camera;
class World;
class ProgramResource;

struct GLbuffer {
	GLuint vertex;
	GLuint index;
};

class GLshader;
class GLprogram;

//typedef std::map<WorldChunk*,GLeometry> ChunkGeomList;
//typedef std::map<WorldChunk*,GLbuffer> ChunkGeomList;

/**
 * Geometry types, used to dermine wether the renderer should use Geometry shaders or push the finished vertex data to the GPU
 * Rational being that if the GPU doesn't support Geometry shaders then it probably couldn't fill all the triangles anyway, so it'll probably have reduced draw distance.
 */
enum GeomType {
	GEOM_FALLBACK = 1,
	GEOM_GEOMSHADER = 2
};

/**
 * Debug typedefs
 */
typedef std::pair<Vector3,Vector3> Line;
typedef std::vector<Line> LineList;

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

	GLprogram* mLightingProgram;
	ProgramResource* mWorldProgram;

	GeomType mGeomType;

	/**
	 * Stores a list of debuging points
	 */
	LineList mDebugLines;

public:
	Renderer(void);
	~Renderer(void);

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
		DEBUG_PHYSICS,
		DEBUG_LINES,
		DEBUG_SKY
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
	 * Adds a line to the debug drawing list
	 */
	void addDebugLine( const Vector3& start, const Vector3& end );

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
	void _renderTree( WorldTree* tree );
	void _renderNode( WorldNode* node, int depth  );
	void _renderChunk( Chunk* chunk );

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
	void notifyChunkUnloaded( Chunk* chunk );

	/**
	 * Sets the internal rendermode:-
	 * * RENDER_SOLID: Render with textures.
	 * * RENDER_WIRE: Render in wireframe.
	 * @param rendermode Rendermode to switch to.
	 */
	void setRenderMode( size_t rendermode );

	size_t getRenderMode();

	/**
	 * Returns the current geometry type
	 */
	GeomType getGeometryType();

	/**
	 * Sets the Debug mode
	 */
	void setDebugMode( size_t debugMode );
};

#endif
