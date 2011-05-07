#include "WorldChunk.h"
#include "StoneBlock.h"

WorldChunk::WorldChunk(long x, long y, long z)
: mX( x ),
mY( y ),
mZ( z ),
mHasChanged( false )
{
	initalize();
}

WorldChunk::~WorldChunk(void)
{
	//TODO: FIX MEMORY LEAK HERE
}

void WorldChunk::initalize()
{
	//do nothing! bet they wern't expecting that.
}

void WorldChunk::fillWithTestData()
{
	if(mY == 0)
	{
		for(int x = 0; x < CHUNK_WIDTH; x++)
		{
			for(int z = 0; z < CHUNK_WIDTH; z++)
			{
				for(int y = 10; y >= 0; y--)
				{
					addBlockToChunk( new StoneBlock(x, y, z) );
				}
			}
		}
	}
}

void WorldChunk::addBlockToChunk(BaseBlock* block)
{
	if( getBlockAt( block->getX(), block->getY(), block->getZ() ) != NULL ) {
		std::cout << "Error: Block already exsists!" << std::endl;
	}
	else {
		mHasChanged = true;
		mBlockData.insert( BlockPosPair( block->getZ() * CHUNK_WIDTH * CHUNK_HEIGHT + block->getY() * CHUNK_WIDTH + block->getX(), block ) );
	}
}

BaseBlock* WorldChunk::getBlockAt(long x, long y, long z)
{
	// For a 3D array stored in 1D array, you must:-
	//  z*height*width + y * width + x
	BlockList::iterator it = mBlockData.find( z * CHUNK_WIDTH * CHUNK_HEIGHT + y * CHUNK_WIDTH + x );
	if( it != mBlockData.end() )
		return it->second;
	return NULL;
}

BlockList* WorldChunk::getBlocks()
{
	return &mBlockData;
}

size_t WorldChunk::getBlockCount()
{
	return mBlockData.size();
}

BlockList* WorldChunk::getVisibleBlocks()
{
	return &mVisibleBlocks;
}

size_t WorldChunk::getVisibleBlockCount()
{
	return mVisibleBlocks.size();
}

void WorldChunk::updateVisibility()
{
	// Just a brute force Occlusion test: perhaps this could be optimized?
	for(BlockList::iterator block = mBlockData.begin(); block != mBlockData.end(); ++block) {
		BaseBlock* b = (*block).second;
		//Check All axes for adjacent blocks.
		if( getBlockAt( b->getX() + 1, b->getY(), b->getZ() ) == NULL )
			_blockVisible( (*block), true );
		else if( getBlockAt( b->getX() - 1, b->getY(), b->getZ() ) == NULL )
			_blockVisible( (*block), true );
		else if( getBlockAt( b->getX(), b->getY() + 1, b->getZ() ) == NULL )
			_blockVisible( (*block), true );
		else if( getBlockAt( b->getX(), b->getY() - 1, b->getZ() ) == NULL )
			_blockVisible( (*block), true );
		else if( getBlockAt( b->getX(), b->getY(), b->getZ() + 1 ) == NULL )
			_blockVisible( (*block), true );
		else if( getBlockAt( b->getX(), b->getY(), b->getZ() + 1 ) == NULL )
			_blockVisible( (*block), true );
		else
			_blockVisible( (*block), false );
	}
}

void WorldChunk::_blockVisible( BlockPosPair &block, bool v )
{
	BlockList::iterator it = mVisibleBlocks.find( block.first );
	if( it == mVisibleBlocks.end() && v ) {
		mVisibleBlocks.insert( block );
	}
	else if( it != mVisibleBlocks.end() && !v )  {
		it = mVisibleBlocks.erase( it );
	}
}

void WorldChunk::update( float dt ) 
{
	if( mHasChanged ) {
		updateVisibility();
		mHasChanged = false;
	}
}

long WorldChunk::getX()
{
	return mX;
}

long WorldChunk::getY()
{
	return mY;
}

long WorldChunk::getZ()
{
	return mZ;
}