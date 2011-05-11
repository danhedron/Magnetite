#include "WorldChunk.h"
#include "StoneBlock.h"
#include "WoodBlock.h"
#include "Renderer.h"

WorldChunk::WorldChunk(long x, long y, long z)
: mX( x ),
mY( y ),
mZ( z ),
mHasChanged( false ),
mHasGenerated( false ),
mGeometry( NULL ),
mVisibleFaces( 0 )
{
	initalize();
}

WorldChunk::~WorldChunk(void)
{
	//TODO: FIX MEMORY LEAK HERE
	for(BlockList::iterator block = mBlockData.begin(); block != mBlockData.end(); ++block) {
		delete block->second;
	}
	mBlockData.clear();
}

void WorldChunk::initalize()
{
	//do nothing! bet they wern't expecting that.
}

void WorldChunk::fillWithTestData()
{
	float d = 0;
	if(mY == 0)
	{
		for(int x = 0; x < CHUNK_WIDTH; x++)
		{
			for(int z = 0; z < CHUNK_WIDTH; z++)
			{
				for(int y = 0; y < 3; y++)
				{
					if( y == 9 )
						addBlockToChunk( new WoodBlock(x, y, z) );
					else
						addBlockToChunk( new StoneBlock(x, y, z) );
				}
			}
		}
	}
}

void WorldChunk::addBlockToChunk(BaseBlock* block)
{
	long k = block->getZ() * CHUNK_WIDTH * CHUNK_HEIGHT + block->getY() * CHUNK_WIDTH + block->getX();
	BlockList::iterator lb = mBlockData.find( k );

	if(lb != mBlockData.end())
	{
		delete lb->second;
		lb->second = block;
	}
	else
	{
		// the key does not exist in the map
		// add it to the map
		mBlockData.insert(lb, BlockList::value_type(k, block));
	}
	mHasChanged = true;
	mHasGenerated = false;
}

void WorldChunk::reserveBlocks( size_t count )
{

}

void WorldChunk::removeBlockAt(long x, long y, long z)
{
	long k = z * CHUNK_WIDTH * CHUNK_HEIGHT + y * CHUNK_WIDTH + x;
	BlockList::iterator lb = mBlockData.lower_bound( k );

	if(lb != mBlockData.end() && !(mBlockData.key_comp()(k, lb->first)))
	{
		delete lb->second;
		mBlockData.erase( lb );
	}
	mHasChanged = true;
	mHasGenerated = false;
}

BaseBlock* WorldChunk::getBlockAt(long x, long y, long z)
{
	// For a 3D array stored in 1D array, you must:-
	//  z*height*width + y * width + x
	if( x < 0 || y < 0 || z < 0 || x == CHUNK_WIDTH || y == CHUNK_HEIGHT || z == CHUNK_WIDTH )
		return NULL;
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

long WorldChunk::getVisibleFaceCount()
{
	return mVisibleFaces;
}

void WorldChunk::updateVisibility()
{
	mVisibleFaces = 0;
	mVisibleBlocks.clear();
	// Just a brute force Occlusion test: perhaps this could be optimized?
	for(BlockList::iterator block = mBlockData.begin(); block != mBlockData.end(); ++block) {
		BaseBlock* b = (*block).second;
		short visFlags = VIS_NONE;
		//Check All axes for adjacent blocks.
		if( getBlockAt( b->getX() + 1, b->getY(), b->getZ() ) == NULL ) {
			mVisibleFaces++;
			visFlags = visFlags | VIS_RIGHT;
		}
		if( getBlockAt( b->getX() - 1, b->getY(), b->getZ() ) == NULL ) {
			mVisibleFaces++;
			visFlags = visFlags | VIS_LEFT;
		}
		if( getBlockAt( b->getX(), b->getY() + 1, b->getZ() ) == NULL ) {
			mVisibleFaces++;
			visFlags = visFlags | VIS_TOP;
		}
		if( getBlockAt( b->getX(), b->getY() - 1, b->getZ() ) == NULL ) {
			mVisibleFaces++;
			visFlags = visFlags | VIS_BOTTOM;
		}
		if( getBlockAt( b->getX(), b->getY(), b->getZ() + 1 ) == NULL ) {
			mVisibleFaces++;
			visFlags = visFlags | VIS_BACK;
		}
		if( getBlockAt( b->getX(), b->getY(), b->getZ() - 1 ) == NULL ) {
			mVisibleFaces++;
			visFlags = visFlags | VIS_FORWARD;
		}
		(*block).second->mViewFlags = visFlags;
		if( visFlags == VIS_NONE )
			_blockVisible( (*block), false );
		else
			_blockVisible( (*block), true);
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

void WorldChunk::generate()
{
	Util::log("Generating chunk mesh");
	if( mGeometry != NULL ) {
		Util::log("Recycling mesh data");
		delete[] mGeometry->edgeData;
		delete[] mGeometry->vertexData;
	}
	GLuint VertexSize = getVisibleFaceCount() * 4 * sizeof(GLvertex);
	GLuint vertexCount	 = getVisibleFaceCount() * 4;
	GLuint edgeCount	 = getVisibleFaceCount() * 12;
	GLvertex* vertexData = new GLvertex[vertexCount];
	GLedge* edgeData	 = new GLedge[edgeCount];

	BlockList* blocks = getVisibleBlocks();
	size_t ind = 0;
	size_t edgeInd = 0;

	for( BlockList::iterator block = mBlockData.begin(); block != mBlockData.end(); ++block )
	{
		Renderer::buildCubeData((*block).second, ind, edgeInd, vertexData, edgeData);
	}
	//Util::log("Highest Index: " + Util::toString(ind) + " Allocated: " + Util::toString(vertexCount) + " Highest Index: " + Util::toString(edgeInd) + " Allocated: " + Util::toString(edgeCount));
	
	// Chunk has been defined, store it's data
	mGeometry = new GLgeometry;
	mGeometry->edgeData = edgeData;
	mGeometry->vertexData = vertexData;
	mGeometry->edgeCount = edgeCount;
	mGeometry->vertexCount = vertexCount;

	notifyGenerated();
	// Delete the chunk's previous data
	//mWorldBuffers.insert( ChunkGeomList::value_type( chunk, geom ) );
}

bool WorldChunk::hasGenerated()
{
	return mHasGenerated;
}

void WorldChunk::notifyGenerated()
{
	mHasGenerated = true;
}

GLgeometry* WorldChunk::getGeometry()
{
	return mGeometry;
}

void WorldChunk::update( float dt ) 
{
	if( mHasChanged ) {
		updateVisibility();
		generate();
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
