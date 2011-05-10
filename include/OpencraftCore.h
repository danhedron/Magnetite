#ifndef _OPENCRAFTCORE_H_
#define _OPENCRAFTCORE_H_
#include "prerequisites.h"

class Renderer;
class WorldChunk;
class TextureManager;

/**
 * @struct Raycast result structure.
 */
struct raycast_r
{
	float i0;
	float i1;
	float maxDistance;
	float length;
	bool  hit;

	Vector3 orig;
	Vector3 dir;
	Vector3 worldHit;

	raycast_r(void) { 
		i0 = std::numeric_limits<float>::max();
		i1 = -std::numeric_limits<float>::max();
		maxDistance = 100.f;
		length = std::numeric_limits<float>::max();
		hit = false;
	}
};

/** @class OpencraftCore
 * Responsible for managing stuff 
 */
class OpencraftCore
{
protected:
	sf::Window	mWindow;
	sf::Clock	mClock;
	Renderer*	mRenderer;
	TextureManager* mTextureManager;
	bool		mContinue;
	ChunkList	mChunks;
	int			radius;
public:
	OpencraftCore(void);
	~OpencraftCore(void);

	/**
	 * Singleton instance of the OpencraftCore
	 */
	static OpencraftCore *Singleton;

	/**
	 * @return a pointer to the Texture Manager
	 */
	TextureManager *getTextureManager();

	/**
	 * Creates a chunk at the given coordinates and fills it with test data
	 * @param x Coordinate
	 * @param y Coordinate
	 * @param z Coordinate
	 */
	void createChunk(long x, long y, long z);
	/**
	 * Removes the chunk at the given offset
	 * @param x Coordinate
	 * @param y Coordinate
	 * @param z Coordinate
	 */
	void removeChunk(long x, long y, long z);

	/**
	 * Creates a series of test chunks of radius size.
	 * @param size Radius
	 */
	void createTestChunks( int size );

	/**
	 * Destroys all chunks in the world.
	 */
	void destoryWorld();

	/**
	 * Performs a raycast test against a single cube
	 * @param ray Raycast object.
	 * @param min Cube minimum.
	 * @param max Cube maximum.
	 */
	raycast_r& raycastCube(raycast_r &ray, Vector3& min, Vector3& max);

	/**
	 * Performs a raytest against the world.
	 */
	raycast_r raycastWorld(raycast_r &ray);

	WorldChunk* getChunk(const long x, const long y, const long z);

	void createWindow(int *argc, char **argv);

	void go(int *argc, char **argv);

	void exit();
};

#endif