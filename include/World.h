#ifndef _WORLD_H_
#define _WORLD_H_
#include "prerequisites.h"
#include "WorldNode.h"

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

/**
 * @struct AABB->AABB collision test 
 */
struct collision_r
{
	Vector3 min1;
	Vector3 max1;

	Vector3 min2;
	Vector3 max2;

	bool collision;

	collision_r() {
		collision = false;
	}
};

class Sky;
class ChunkGenerator;
class Camera;

class World
{
protected:
	ChunkList	mChunks;
	NodeList	mQuadTrees;
	Sky*		mSky;
	ChunkGenerator* mGenerator;
	Camera*		mPagingCamera;

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
	static Vector3 worldToChunks( const Vector3& vec );

	/**
	 *	Converts world coordinates into block offset ( in the chunk at that location )
	 */
	static Vector3 worldToBlock( const Vector3& vec );

	/**
	 * Returns the top-level Quadtree node at the given coordinates or NULL if it's not in memory
	 * @param Vector3 pos The position to use
	 * @param safe In the case that the leaf doesn't exist and this is true, the leaf will be created for you.
	 */
	WorldNode* getWorldNode( const Vector3& pos, bool safe = false );

	/**
	 * Returns the chunk node at the given position
	 */
	WorldNode* getChunkNode( const Vector3& pos, bool safe = false );
	void _populateTree( WorldNode* parent, const NodeIndex &ppos, int depth );

	std::string printTree();
	std::string _printTree(WorldNode* node, int depth);

	/**
	 * Returns the list of top-level nodes
	 */
	NodeList& getTopNodes();

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
	 * Activates a chunk, loading it if it is stored on disk, or generating it if necassery
	 */
	void activateChunk( long x, long y, long z );

	/**
	 * Deactivates a chunk
	 */
	void deativateChunk( long x, long y, long z );

	/**
	 * Sets the camera used for paging chunks
	 */
	void setPagingCamera( Camera* _c );

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
	raycast_r raycastWorld(const raycast_r &inray, bool solidOnly = false);

	/**
	 * Returns the chunk at the given indexes.
	 */
	WorldChunk* getChunk(const long x, const long y, const long z);

	/**
	 * Performs an AABB test against the world.
	 */
	collision_r AABBWorld(const collision_r& info );

	collision_r AABBCube(const collision_r& info );

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