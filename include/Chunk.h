#ifndef _CHUNK_H_
#define _CHUNK_H_
#include "prerequisites.h"
#include "Region.h"
#include <mutex>

// for standard size types
#include <cstdint>

class TerrainGeometry;
class World;

typedef std::map<size_t, BlockPtr> BlockList;

class Chunk
{
protected:
	/**
	 * World Pointer
	 */
	World*	mWorld;
	
	/**
	 * Array of blocks
	 */
	BlockArray	mBlocks;
	
	/**
	 * List of blocks that are visible
	 */
	BlockList	mVisibleBlocks;

	/**
	 * Index of the chunk in the world
	 */
	ChunkIndex	mWorldIndex;
	
	/**
	 * Array of light values
	 */
	LightIndex* mLightValues;
	
	/**
	 * Number of visible faces.
	 */
	size_t	mVisibleFaces;
	
	/**
	 * The chunk's geometry
	 */
	TerrainGeometry*	mGeometry;

	/**
	 * Allocates the array of a given size
	 */
	void _allocateArray( size_t size );

	/**
	 * Chunk flags
	 */
	uint16_t mChunkFlags;

	/**
	 * Blocks to be deleted
	 */
	BlockList mDeletedBlocks;
	
	/**
	 * Physics Objects
	 */
	btCollisionShape*		mPhysicsShape;
	btDefaultMotionState*	mPhysicsState;
	btTriangleMesh*		mPhysicsMesh;
	btRigidBody*		mPhysicsBody;
	
	/**
	 * Block counter
	 */
	size_t mNumBlocks;
	
	/**
	 * Threading lock
	 *  ensures that only one thread is doing something with this chunk.
	 */
	std::mutex mMutex;

public:
	/**
	 * Flags Enum
	 */
	enum {
		DataUpdated = 1, // Data has been updated, Visibility check needed
		MeshInvalid = 2, // Data has changed, mesh should be updated.
		SkipLight = 4 // Skips updating lighting for one update.
	};
	
	/**
	 * Method for getting the block directly from us if it's inside this chunk
	 */
	inline BaseBlock* getBlockAtWorld( ChunkScalar x, ChunkScalar y, ChunkScalar z );

	Chunk( ChunkIndex index, World*  world );
	~Chunk();

	/**
	 * Returns the chunk's X index.
	 */
	long getX();
	
	/**
	 * Returns the chunks Y index
	 */
	long getY();

	/**
	 * Returns the chunks z index
	 */
	long getZ();
	
	/**
	 * Returns the world this chunk belongs to.
	 */
	World* getWorld();
	
	/**
	 * Inserts the block at the given position
	 */
	inline void setBlockAt( BlockPtr block, ChunkScalar x, ChunkScalar y, ChunkScalar z )
	{
		setBlockAt( block, BLOCK_INDEX_2( x, y, z ) );
	}

	/**
	 * Inserts the block at the given index
	 */
	inline void setBlockAt( BlockPtr block, ChunkScalar index )
	{
		if( index > CHUNK_SIZE || index < 0 ) return;
		if( mBlocks[ index ] != NULL )
		{
			removeBlockAt( index );
		}
		// Lock here to avoid locking the thread.
		getMutex().lock();
		mNumBlocks++;
		mBlocks[ index ] = block;
		_raiseChunkFlag( DataUpdated );
		getMutex().unlock();
	}

	/**
	 * Removes a block at the given position 
	 */
	void removeBlockAt( short x, short y, short z );

	/**
	 * Removes the block at the given index
	 */
	void removeBlockAt( short index );

	/**
	 * Returns the block at the given position
	 */
	inline BlockPtr getBlockAt( ChunkScalar x, ChunkScalar y, ChunkScalar z )
	{
		if( x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_WIDTH )
			return NULL;
		return getBlockAt( BLOCK_INDEX_2( x, y, z ) );
	}
	/**
	 * Returns the block at the given index
	 */
	inline BlockPtr getBlockAt( const size_t index )
	{
		// No bounds checking, goota be careful.
		return mBlocks[ index ];
	}


	/**
	 * Returns all of the blocks
	 */
	BlockPtr* getBlocks();

    /**
     * Returns the number of visisble faces this chunk has
     */
	size_t getVisibleFaceCount();
	
	/**
	 * Returns the list of visible blocks
	 */
	BlockList& getVisibleBlocks();

	/**
	 * Retuns true if there's a block next to the given position
	 */
	bool hasNeighbours( long int x, long int y, long int z );
	
	/** 
	 * Sets the light value at the given pointer
	 */
	void setLightLevel( LightIndex value, short x, short y, short z ) {
		if( x >= 0 && x < CHUNK_WIDTH && y >= 0 && y < CHUNK_HEIGHT && z >= 0 && z < CHUNK_WIDTH )
			mLightValues[ BLOCK_INDEX_2( x, y, z ) ] = value;
	}
	
	/**
	 * Sets the light level for the given index.
	 */
	void setLightLevel( LightIndex value, ChunkScalar ind ) {
		if( ind >= 0 && ind < CHUNK_SIZE ) { 
			mLightValues[ ind ] = value;
		}
	}
	
	/**
	 * returns the light level at the block
	 */
	LightIndex getLightLevel( ChunkScalar x, ChunkScalar y, ChunkScalar z )
	{
		if( x >= 0 && x < CHUNK_WIDTH && y >= 0 && y < CHUNK_HEIGHT && z >= 0 && z < CHUNK_WIDTH )
			return mLightValues[ BLOCK_INDEX_2( x, y, z ) ];
		return 255;
	}

	/**
	 * Gets the light level for the given index.
	 */
	LightIndex getLightLevel( ChunkScalar ind ) {
		if( ind >= 0 && ind < CHUNK_SIZE ) { 
			return mLightValues[ ind ];
		}
		return 255;
	}
	
	/**
	 * Returns a pointer to this chunk's geometry
	 */
	Geometry* getGeometry();
	
	/**
	 * Requests that the chunk re-generate
	 */
	void requestGenerate();
	
	/**
	 * Generates the chunk's actual geometry
	 */
	void generate();
	
	/**
	 * Generates the geometry for this chunk's mesh.
	 */
	void generateGeometry();
	
	/**
	 * Generates the light data for this chunk
	 */
	void generateLighting();
	
	/**
	 * Generates the chunk's physical geometry
	 */
	void generatePhysics();
	
	/**
	 * Updates the visibility flags of each block
	 */
	void updateVisibility();

	/**
	 * Updates the chunk
	 */
	void update( float dt );

	/**
	 * Raises a given flag on this chunk
	 */
	void _raiseChunkFlag( uint16_t flag );

	/**
	 * Returns true if the given flag is raised
	 */
	bool _hasChunkFlag( uint16_t flag );
	
	/**
	 * Removes a chunk flag
	 */
	void _lowerChunkFlag( uint16_t flag );
	
	/**
	 * Returns the number of non-empty blocks in the chunk
	 */
    const size_t getBlockCount();
	
	/**
	 * Returns the mutex for this chunk
	 */
	std::mutex& getMutex();
};

#endif
