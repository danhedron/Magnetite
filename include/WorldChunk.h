#ifndef _WORLDCHUNK_H_
#define _WORLDCHUNK_H_
#include "prerequisites.h"
#include <vector>
#include <istream>

class BaseBlock;
struct GLgeometry;

class WorldChunk
{
protected:
	BlockList mBlockData;
	BlockList mVisibleBlocks; //< Used in Rendering
	long mX;
	long mY;
	long mZ;
	bool mHasChanged;
	bool mHasGenerated;
	long mVisibleFaces;
	void _blockVisible( BlockPosPair &block, bool v );
	GLgeometry* mGeometry;
public:
	WorldChunk(long x, long y, long z);
	~WorldChunk(void);

	/**
	 * Resets all data in the chunk to air.
	 */
	void initalize();

	/**
	 * Fills the chunk with test data
	 */
	void fillWithTestData();

	/**
	 * Reserve items in the internal memory for large inserts.
	 * @param count The Number of blocks you are going to insert.
	 */
	void reserveBlocks(size_t count);

	/**
	 * Adds an already created block to the chunk.
	 * @param block pointer to block
	 */
	void addBlockToChunk(BaseBlock* block);

	/**
	 * Removes the block at the specifed location
	 *
	 */
	void removeBlockAt(long x, long y, long z);

	/**
	 * Returns a block if one exsists at the given position.
	 * @param x X coordinate.
	 * @param y Y coordinate.
	 * @param z Z Coordinate.
	 * @return The block at this coordinate, or NULL
	 */
	BaseBlock* getBlockAt(long x, long y, long z);

	/**
	 * Returns a pointer to the block list
	 */
	BlockList* getBlocks();

	/**
	 * Returns a pointer to the Visible block list
	 */
	BlockList* getVisibleBlocks();

	/**
	 * Returns the block count
	 */
	size_t getBlockCount();

	/**
	 * Returns the visible block count
	 */
	size_t getVisibleBlockCount();

	/** 
	 * Returns the number of visible faces
	 */
	long getVisibleFaceCount();

	/**
	 * Performs a visibility test on all blocks in this chunk
	 */
	void updateVisibility();

	/**
	 * Returns wether or not this chunk has had it's geometry 
	 */
	bool hasGenerated();

	/**
	 * Tells the chunk it's geometry has been cooked.
	 */
	void notifyGenerated();

	/**
	 * Generate geometry for this CHUNK
	 */
	void generate();

	/**
	 * Returns this chunk's geometry data
	 */
	GLgeometry* getGeometry();

	/**
	 * Performs updates on things inside the chunk.
	 * @param dt The time since the last update.
	 */
	void update( float dt );

	/**
	 * Returns the X index of this chunk.
	 */
	long getX();

	/**
	 * Returns the Y Index of this chunk.
	 */
	long getY();
	
	/**
	 * Returns the Z index of this chunk.
	 */
	long getZ();
};

#endif