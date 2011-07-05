#ifndef _WORLDCHUNK_H_
#define _WORLDCHUNK_H_
#include "prerequisites.h"
#include <vector>
#include <iostream>
#include <fstream>

class BaseBlock;
struct GLgeometry;

class WorldChunk
{
protected:
	BlockArray mBlockData;
	BlockList mVisibleBlocks; //< Used in Rendering
	BlockList mThinkingBlocks; //< Used in updates
	BlockList mShouldDelete; //< Used to allow blocks to remove themselves
	long mX;
	long mY;
	long mZ;
	bool mHasChanged;
	bool mHasGenerated;
	long mVisibleFaces;
	void _blockVisible( BlockPtr &block, bool v );
	GLgeometry* mGeometry;
	float mUpdateTimer;
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
	 * Adds a block to the delete list so that a block can delete itself
	 */
	void _addBlockToRemoveList(BaseBlock* block);

	/**
	 * returns a relative chunk
	 */
	WorldChunk* getRelativeChunk( unsigned short x, unsigned short y, unsigned short z );

	/**
	 * Returns a block if one exsists at the given position.
	 * @param x X coordinate.
	 * @param y Y coordinate.
	 * @param z Z Coordinate.
	 * @return The block at this coordinate, or NULL
	 */
	BaseBlock* getBlockAt(long x, long y, long z);

	/**
	 * Called when a block inside the chunk is moved.
	 */
	void _blockMoved( BaseBlock* block, short x, short y, short z );

	/**
	 * Returns a pointer to the block list
	 */
	BlockArray* getBlocks();

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
	 * Tells the chunk it's geometry may have been changed
	 */
	void markModified();

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
	 * Requests that the chunk generate it's geometry if hasn't already been generated
	 */
	void requestGenerate();

	/**
	 * Appends this chunk's data to an ofstream
	 */
	void appendToStream( std::ofstream& stream );

	/**
	 * Reads blocks out of a stream.
	 */
	void readFromStream( std::ifstream& stream );

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