#include "WorldChunk.h"
#include "BaseBlock.h"
#include "WaterBlock.h"
#include "BlockFactory.h"
#include "Renderer.h"
#include "OpencraftCore.h"
#include "World.h"

WorldChunk::WorldChunk(long x, long y, long z)
: mX( x ),
mY( y ),
mZ( z ),
mHasChanged( false ),
mHasGenerated( false ),
mGeometry( NULL ),
mVisibleFaces( 0 ),
mUpdateTimer( 0 )
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
				for(int y = 0; y < 10; y++)
				{
					BaseBlock* block = NULL;
					if( y == 9 )
						block = FactoryManager::createBlock("grass");
					else if( y == 8 || y == 7 )
						block = FactoryManager::createBlock("dirt");
					else
						block = FactoryManager::createBlock("stone");
					block->setPosition( x, y, z );
					addBlockToChunk( block );
				}
			}
		}
	}
}

void WorldChunk::addBlockToChunk(BaseBlock* block)
{
	WorldChunk* chunk = getRelativeChunk( block->getX(), block->getY(), block->getZ() );
	if( chunk != this ) {
		if( chunk == NULL ) {
			delete block;
			return;
		}
		if( block->getX() < 0 )
			block->setPosition( Vector3( block->getX() + CHUNK_WIDTH, block->getY(), block->getZ() ) );
		else if( block->getX() >= CHUNK_WIDTH )
			block->setPosition( Vector3( block->getX() - CHUNK_WIDTH, block->getY(), block->getZ() ) );
		if( block->getY() < 0 )
			block->setPosition( Vector3( block->getX(), block->getY() + CHUNK_HEIGHT, block->getZ() ) );
		else if( block->getY() >= CHUNK_HEIGHT )
			block->setPosition( Vector3( block->getX(), block->getY() - CHUNK_HEIGHT, block->getZ() ) );
		if( block->getZ() < 0 )
			block->setPosition( Vector3( block->getX(), block->getY(), block->getZ() + CHUNK_WIDTH ) );
		else if( block->getZ() >= CHUNK_WIDTH )
			block->setPosition( Vector3( block->getX(), block->getY(), block->getZ() - CHUNK_WIDTH ) );
		chunk->addBlockToChunk( block );
		return;
	}
	long k = BLOCK_INDEX( block );
	BlockList::iterator lb = mBlockData.find( k );

	block->_setChunk( this );

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

void WorldChunk::_blockMoved( BaseBlock* block, short x, short y, short z )
{
	long k = BLOCK_INDEX( block );
	long kn = z * CHUNK_WIDTH * CHUNK_HEIGHT + y * CHUNK_WIDTH + x;
	BlockList::iterator lb = mBlockData.find( k );
	removeBlockAt( x, y, z );
	if( lb != mBlockData.end() )
		mBlockData.erase( lb++ );
	mBlockData.insert( BlockList::value_type( kn, block ) );
	markModified();
}

void WorldChunk::reserveBlocks( size_t count )
{

}

void WorldChunk::removeBlockAt(long x, long y, long z)
{
	WorldChunk* chunk = getRelativeChunk( x, y, z );
	if( chunk != this ) {
		if( x < 0 )
			x += CHUNK_WIDTH;
		else if( x >= CHUNK_WIDTH )
			x -= CHUNK_WIDTH;
		if( y < 0 )
			y += CHUNK_HEIGHT;
		else if( y >= CHUNK_HEIGHT )
			y -= CHUNK_HEIGHT;
		if( z < 0 )
			z += CHUNK_WIDTH;
		else if( z >= CHUNK_WIDTH )
			z -= CHUNK_WIDTH;
		chunk->removeBlockAt( x, y, z );
		return;
	}
	long k = z * CHUNK_WIDTH * CHUNK_HEIGHT + y * CHUNK_WIDTH + x;
	BlockList::iterator it = mBlockData.find( k );
	if( it != mBlockData.end() )  {
		delete (*it).second;
		(*it).second = NULL;
		_blockVisible( *it, false );
		mBlockData.erase( it++ );
	}
	mHasChanged = true;
	mHasGenerated = false;
}

void WorldChunk::_addBlockToRemoveList(BaseBlock* block)
{
	mShouldDelete.insert( BlockList::value_type( BLOCK_INDEX( block ) , block ) );
}

WorldChunk* WorldChunk::getRelativeChunk(long x, long y, long z)
{
	if( ( x >= 0 && y >= 0 && z >= 0 ) && ( x < CHUNK_WIDTH && y < CHUNK_HEIGHT && z < CHUNK_WIDTH ) )
		return this;

	/*if( x < 0 )
		x += CHUNK_WIDTH;
	else if( x >= CHUNK_WIDTH ) 
		x -= CHUNK_WIDTH;
	if( y < 0 )
		y += CHUNK_HEIGHT;
	else if( y >= CHUNK_HEIGHT )
		y -= CHUNK_HEIGHT;
	if( z < 0 )
		z += CHUNK_WIDTH;
	else if( z >= CHUNK_WIDTH )
		z -= CHUNK_WIDTH;*/

	Vector3 chunk = World::worldToChunks( Vector3( ( mX * CHUNK_WIDTH ) + x , ( mY * CHUNK_HEIGHT ) + y , ( mZ * CHUNK_WIDTH ) + z ) );
	return OpencraftCore::Singleton->getWorld()->getChunk( chunk.x, chunk.y, chunk.z );
	//return chunk;
}

BaseBlock* WorldChunk::getBlockAt(long x, long y, long z)
{
	// For a 3D array stored in 1D array, you must:-
	//  z*height*width + y * width + x
	WorldChunk* chunk = getRelativeChunk( x, y, z );
	if( chunk != this ) {
		// If the coordinates fall outside of this chunk, silently return the chunk containing that relative block.
		WorldChunk* relChunk = getRelativeChunk( x, y, z );
		if( x < 0 )
			x += CHUNK_WIDTH;
		else if( x >= CHUNK_WIDTH )
			x -= CHUNK_WIDTH;
		if( y < 0 )
			y += CHUNK_HEIGHT;
		else if( y >= CHUNK_HEIGHT )
			y -= CHUNK_HEIGHT;
		if( z < 0 )
			z += CHUNK_WIDTH;
		else if( z >= CHUNK_WIDTH )
			z -= CHUNK_WIDTH;
		if( relChunk )
			return relChunk->getBlockAt(x, y, z);
		else 
			return NULL;
		//return NULL;
	}
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
		short visFlags = FACE_NONE;
		short visOrig = b->mViewFlags;
		//Check All axes for adjacent blocks.
		BaseBlock* cb = getBlockAt( b->getX() + 1, b->getY(), b->getZ() );
		if( cb == NULL || !cb->isOpaque() ) {
			mVisibleFaces++;
			visFlags = visFlags | FACE_RIGHT;
		}
		cb = getBlockAt( b->getX() - 1, b->getY(), b->getZ() );
		if( cb == NULL || !cb->isOpaque() ) {
			mVisibleFaces++;
			visFlags = visFlags | FACE_LEFT;
		}
		cb = getBlockAt( b->getX(), b->getY() + 1, b->getZ() );
		if( cb == NULL || !cb->isOpaque() ) {
			mVisibleFaces++;
			visFlags = visFlags | FACE_TOP;
		}
		cb = getBlockAt( b->getX(), b->getY() - 1, b->getZ() );
		if( cb == NULL || !cb->isOpaque() ) {
			mVisibleFaces++;
			visFlags = visFlags | FACE_BOTTOM;
		}
		cb = getBlockAt( b->getX(), b->getY(), b->getZ() + 1 );
		if( cb == NULL || !cb->isOpaque() ) {
			mVisibleFaces++;
			visFlags = visFlags | FACE_BACK;
		}
		cb = getBlockAt( b->getX(), b->getY(), b->getZ() - 1 );
		if( cb == NULL || !cb->isOpaque() ) {
			mVisibleFaces++;
			visFlags = visFlags | FACE_FORWARD;
		}
		(*block).second->mViewFlags = visFlags;
		for( size_t f = 0; f < 6; f++ ) {
			if( ((1<<f) & visOrig ) == (1<<f) ) {
				if( ( visOrig & visFlags ) != (1<<f) ) {
					b->connectedChange( (1<<f) );
				}
			}
		}
		if( visFlags == FACE_NONE )
			_blockVisible( (*block), false );
		else
			_blockVisible( (*block), true);
	}
}

void WorldChunk::_blockVisible( BlockPosPair &block, bool v )
{
	BlockList::iterator it = mVisibleBlocks.find( block.first );
	if( v && it == mVisibleBlocks.end() ) {
		mVisibleBlocks.insert( block );
	}
	else if( !v && it != mVisibleBlocks.end() )  {
		mVisibleBlocks.erase( it++ );
	}
}

void WorldChunk::generate()
{
	Util::log("Generating chunk mesh");
	if( mGeometry != NULL ) {
		delete[] mGeometry->vertexData;
		delete[] mGeometry->edgeData;
	}
	GLuint vertexCount	 = getVisibleFaceCount() * 4;
	GLuint edgeCount	 = getVisibleFaceCount() * 6;
	GLvertex* vertexData = new GLvertex[vertexCount];
	GLedge* edgeData	 = new GLedge[edgeCount];

	size_t ind = 0;
	size_t edgeInd = 0;

	for( BlockList::iterator block = mVisibleBlocks.begin(); block != mVisibleBlocks.end(); ++block )
	{
		block->second->buildCubeData(ind, edgeInd, vertexData, edgeData);
	}
	
	// Chunk has been defined, store it's data
	if( mGeometry == NULL )
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

void WorldChunk::markModified()
{
	mHasChanged = true;
	mHasGenerated = false;
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
	while( mUpdateTimer >= 0.1f ) {
		mUpdateTimer -= 0.1f;
		for( BlockList::iterator it = mBlockData.begin(); it != mBlockData.end(); ++it ) {
			if( it->second->isFluid() ) {
				((WaterBlock*)it->second)->flow( 0.1f );
			}
		}
	}
	mUpdateTimer += dt;

	for( BlockList::iterator it = mShouldDelete.begin(); it != mShouldDelete.end(); ) {
		removeBlockAt( it->second->getX(), it->second->getY(), it->second->getZ() );
		mShouldDelete.erase( it++ );
	}
}

void WorldChunk::requestGenerate()
{
	if( mHasChanged ) {
		updateVisibility();
		if( !mHasGenerated )
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
