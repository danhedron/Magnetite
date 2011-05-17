#ifndef _WORLD_H_
#define _WORLD_H_
#include "prerequisites.h"

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

	BaseBlock* block;

	raycast_r(void) { 
		i0 = std::numeric_limits<float>::max();
		i1 = -std::numeric_limits<float>::max();
		maxDistance = 100.f;
		length = std::numeric_limits<float>::max();
		hit = false;
		block = NULL;
	}
};

class Sky;

class World
{
protected:
	ChunkList	mChunks;
	Sky*		mSky;

public:
	/** 
	 * Constructor: -
	 */
	World( void );
	
	/**
	 * Destructor:- 
	 */
	~World( void );

	/**
	 * Loads a world from the specified location
	 */
	void loadWorld( std::string location );

	/**
	 * Creates a new empty world.
	 */
	void createWorld();

	/**
	 * Returns a reference to the chunk list.
	 */
	ChunkList& getChunks();
	
	/**
	 *	Converts world coordinates into chunk indexes.
	 */
	Vector3 worldToChunks( const Vector3& vec );

	/**
	 *	Converts world coordinates into block offset ( in the chunk at that location )
	 */
	Vector3 worldToBlock( const Vector3& vec );

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
	 * Updates all loaded chunks
	 */
	void update( float dt );

	/**
	 * Performs a raycast test against a single cube
	 * @param ray Raycast object.
	 * @param min Cube minimum.
	 * @param max Cube maximum.
	 */
	raycast_r raycastCube(const raycast_r &ray, Vector3& min, Vector3& max);

	/**
	 * Performs a raytest against the world.
	 */
	raycast_r raycastWorld(const raycast_r &inray);

	/**
	 * Returns the chunk at the given indexes.
	 */
	WorldChunk* getChunk(const long x, const long y, const long z);

	/**
	 * Creates a new sky object 
	 */
	void createSky( size_t time );

	/**
	 * @returns a Pointer to the sky object.
	 */
	Sky* getSky();

};

#endif // _WORLD_H_