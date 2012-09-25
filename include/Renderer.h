#ifndef _RENDERER_H_
#define _RENDERER_H_
#include "prerequisites.h"
#include "Chunk.h"
#include "Profiler.h"

class Texture;
class BaseBlock;
class Camera;
class World;
class ProgramResource;

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

	ProgramResource* mWorldProgram;
	Texture* mWorldTexture;
	Texture* mCrosshair;
	
	GeomType mGeomType;

	/**
	 * Stores a list of debuging points
	 */
	LineList mDebugLines;

public:
	Renderer(void);
	~Renderer(void);

	/**b
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

	void render( double dt, World* world );

	void _renderChunk( Magnetite::ChunkRegionPtr region, Chunk* chunk );

	/**
	 * Draws debug statistics onto the screen
	 */
	void drawStats( double dt, size_t chunkCount, World* world );
	
	/**
	 * Draws debug information from the profiler on screen.
	 */
	void drawPerf( size_t id, Perf::Profiler& profiler );

	/**
	 * Draws the crosshair
	 */
	void drawCrosshair( double dt );

	/**
	 * Draws the string on the screen (coords start from top left)
	 * Will split each line into a new draw call
	 * @param text Text to draw
	 * @param x Coordinate 
	 * @param y Coordinate
	 */
	void drawText( std::string text, int x, int y, const Vector3& colour = Vector3(0.f, 1.f, 0.f) );

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
