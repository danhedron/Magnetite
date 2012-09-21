#ifndef _WORLD_H_
#define _WORLD_H_
#include "prerequisites.h"
#include "WorldNode.h"
#include "Collision.h"
#include "MovingBlock.h"
#include "Region.h"
#include "paging/PagingContext.h"

namespace Magnetite {
class WorldSerializer;}

class BaseTriangulator;
class Chunk;
typedef Chunk* ChunkPtr;
typedef Chunk** ChunkArray;

/**
 * @struct Raycast result structure.
 */
struct raycast_r
{
	/**
	 * First intersection length
	 */
	float i0;
	/**
	 * Second intersection length
	 */
	float i1;
	/**
	 * Maximum ray length
	 */
	float maxDistance;
	/**
	 * Unused
	 */
	float length;
	/**
	 * True if the ray hit something
	 */
	bool  hit;

	/**
	 * Origin position of the ray
	 */
	Vector3 orig;
	/**
	 * Ray Direction
	 */
	Vector3 dir;
	/**
	 * World Hit Position
	 */
	Vector3 worldHit;
	/**
	 * Hit normal
	 */
	Vector3 hitNormal;

	/**
	 * pointer to block the ray hit, if applicable
	 */
	BaseBlock* block;
	/**
	 * Index of the block that was hit (relative to it's chunk)
	 */
	size_t blockIndex;
	/**
	 * Position of the block, relative to the chunk
	 */
	Vector3 blockPosition;
	/**
	 * Chunk containing the hit block
	 */
	Chunk* chunk;

	raycast_r(void) { 
		i0 = std::numeric_limits<float>::max();
		i1 = -std::numeric_limits<float>::max();
		maxDistance = 100.f;
		length = std::numeric_limits<float>::max();
		hit = false;
		block = NULL;
		chunk = NULL;
		blockIndex = 0;
	}
};

/**
 * @struct AABB->AABB collision test 
 */
struct collision_r
{
	Vector3 response;

	bool collision;

	collision_r() {
		collision = false;
	}
};

class Sky;
class ChunkGenerator;
class Camera;

struct ChunkRequest
{
	int x, y, z;
};

typedef std::vector<ChunkRequest> ChunkLoadList;

/**
* World Stage ENUM, used for determining certain behaviours
* During WORLD_GEN, adjacent chunks are NOT updated if an edge update occurs
*/
enum WorldStage {
	WORLD_GEN = 1, // World is being mass-generated
	WORLD_NORMAL = 2 // World is being run as normal
};


/**
 * ChunkArray - an array of Chunks
 */
typedef Chunk** ChunkArray;

class World : public Magnetite::PagingContext
{
protected:
	
	//ChunkArray	mChunks;
	
	Magnetite::ChunkRegionArray mRegions;
	
	// Size of the world, in regions.
	size_t 		mWorldSize;

	Sky*		mSky;
	ChunkGenerator* mGenerator;
	Camera*		mPagingCamera;
	std::string mWorldName;
	WorldStage mWorldStage;
	ChunkLoadList mChunksToLoad;
	
	MovingBlockList mMovingBlocks;
	
	/**
	 * List of entities in the world.
	 */
	Magnetite::EntityList mEntities;
	
	/**
	 * The Triangulator to use for creating the world geometry
	 */
	BaseTriangulator* mTriangulator;
	
	/**
	 * Serializer, for writing chunks to disk
	 */
	Magnetite::WorldSerializer* mSerializer;
	
public:
	/** 
	 * Constructor: -
	 */
	World( size_t edgeSize );
	
	/**
	 * Destructor:- 
	 */
	~World( void );

	/**
	 * Converts coordinates into indexes 
	 */
	size_t coordsToIndex( int x, int y, int z );
	
	/**
	 * Returns the number of regions.
	 */
	size_t getRegionCount() const;

	/**
	 * Returns the world's current stage
	 */
	WorldStage getCurrentStage();
	
	/**
	 * Returns the triangulator to use for generating meshes.
	 */
	BaseTriangulator* getTriangulator();

	/**
	 * Returns the color of a brightness level
	 */
	static float getLightColor( LightIndex light );
	
	/**
	 * Returns the block at the world index specified
	 */
	BlockPtr getBlockAt( long x, long y, long z );
	
	/**
	 * Removes any block at the given position
	 */
	void removeBlockAt( long x, long y, long z );
	
	/**
	 * Places a block at the world coordinates
	 */
	void setBlockAt( BaseBlock* b, long x, long y, long z );
	
	/**
	 * Starts a block moving if there is a block at the given coordinates
	 */
	void moveBlock( long x, long y, long z, float time, long ex, long ey, long ez );
	
	/**
	 * Returns the list of moving blocks
	 */
	MovingBlockList& getMovingBlocks();
	
	/** 
	 * Returns the light level at the world coordinates
	 * @param x X Coordinate
	 * @param y Y Coordinate
	 * @param z Z Coordinate
	 */
	LightIndex getLightLevel( long x, long y, long z );

	/**
	 * Loads a world of the specified name
	 */
	void loadWorld( std::string name );

	/**
	 * Makes a NEW WORLD!
	 */
	void newWorld( std::string name );

	/**
	 * Returns the name of the world
	 */
	Magnetite::String getName();
	
	/**
	 * Sets the name of the world.
	 */
	void setName( const Magnetite::String& name );

	/**
	 * Returns the path to the current world
	 */
	std::string getSavePath();

	/**
	 * Creates a new empty world.
	 */
	void createWorld();
	
	/**
	 * Creates a new entity of the given type and adds it to the entity pool.
	 */
	template<class T> T* createEntity()
	{
		T* e = new T(this);
		mEntities.push_back(e);
		return e;
	};
	
	Magnetite::EntityList getEntities();
	
	bool printDbg;

	/**
	 * Requests that the engine load or generate the chunk at the given index
	 */
	void requestChunk( int x, int y, int z );

	/**
	 * Returns the top-level Quadtree node at the given coordinates or NULL if it's not in memory
	 * @param Vector3 pos The position to use
	 * @param safe In the case that the leaf doesn't exist and this is true, the leaf will be created for you.
	 */
	WorldNode* getWorldNode( const Vector3& pos, bool safe = false );

	/**
	 * Returns the WorldTree containing the given chunk index
	 * @param Vector3 index The Index being searched for
	 * @param bool create if true then the tree required to store a chunk at this point is created.
	 */
	WorldTree* getWorldTree( const Vector3& index, bool create = false );

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
	TreeList& getTopNodes();

	/**
	 * Creates a chunk at the given coordinates.
	 * @param x Coordinate
	 * @param y Coordinate
	 * @param z Coordinate
	 */
	Chunk* createChunk(long x, long y, long z);
	
	/**
	 * Generates data for the chunk at the given coordinates
	 * @param x Coordinate.
	 * @param y Coordinate.
	 * @param z Coordinate.
	 */
	Chunk* generateChunk( ChunkScalar x, ChunkScalar y, ChunkScalar z );

	/**
	 * Removes the chunk at the given offset
	 * @param x Coordinate
	 * @param y Coordinate
	 * @param z Coordinate
	 */
	void removeChunk(long x, long y, long z);
	
	/**
	 * Creates a Region at the given coordinates
	 */
	Magnetite::ChunkRegionPtr createRegion( const ChunkScalar x, const ChunkScalar y, const ChunkScalar z );
	
	/**
	 * Returns all of the Regions.
	 */
	Magnetite::ChunkRegionArray getRegions() const;

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
	 * Saves all chunks to disk
	 */
	void saveAllChunks();

	/**
	 * Creates the folder structure for the current world
	 */
	void createWorldFolder();

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
	 * Updates all of the moving blocks
	 */
	void updateMovingBlocks( float dt );

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
	Chunk* getChunk(const long x, const long y, const long z);
	
	/**
	 * Returns the Region at the given indexes 
	 */
	Magnetite::ChunkRegionPtr getRegion(const ChunkScalar x, const ChunkScalar y, const ChunkScalar z);

	/**
	 * Performs an AABB test against the world.
	 */
	CollisionResponse AABBWorld( Vector3& min, Vector3& max );

	collision_r AABBCube( Vector3& min, Vector3& max, Vector3& minb, Vector3& maxb );

	/**
	 * Creates a new sky object 
	 */
	void createSky( size_t time );

	/**
	 * @returns a Pointer to the sky object.
	 */
	Sky* getSky();
	
	/**
	 * Builds the data for the initial world
	 */
    void buildTerrain();
	
	/**
	 * Returns true if there is a block surrounding this point
	 */
    bool hasNeighbours(short int x, short int y, short int z);
	
	/**
	 * Called by the PagingContext when a new chunk is needed to be loaded.
	 */
	virtual void onPageEntered(const Magnetite::PageInfo& pageinfo);
	
	/**
	 * Called by the PagingContext when a page should be unloaded.
	 */
	virtual void onPageExit(const Magnetite::PageInfo& pageinfo);

};

#endif // _WORLD_H_
