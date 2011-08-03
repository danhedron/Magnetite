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
	LightIndex mLightValues[CHUNK_SIZE];
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
	 * Public Enumerators
	 */
	enum LightInfo {
		Sunlight = ( 1<<(sizeof(char) * 7))
	};

	/**
	 * Resets all data in the chunk to air.
	 */
	void initalize();

	/**
	 * Fills the chunk with test data
	 */
	void fillWithTestData();

	/**
	 * Sets the light level for the index specifed.
	 */
	void setLightLevel( short x, short y, short z, LightIndex level );

	/**
	 * Returns the lightlevel for the given index.
	 */
	LightIndex getLightLevel( short x, short y, short z );

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
	WorldChunk* getRelativeChunk( short x, short y, short z );

	/**
	 * Tells surrounding chunks they need to update
	 */
	void updateSurrounding();

	/**
	 * Returns true if the given index is at the edge of the chunk
	 */
	bool isEdge( short x, short y, short z );

	/**
	 * Returns true if the given block has neighbours.
	 */
	bool hasNeighbours( short x, short y, short z );

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
	 * Force this chunk to re-cook it's geometry next update
	 */
	void forceGenerate();

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