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
	mBlockData = new BlockPtr[CHUNK_SIZE];
	initalize();
}

WorldChunk::~WorldChunk(void)
{
	//TODO: FIX MEMORY LEAK HERE
	for( size_t i = 0; i < CHUNK_SIZE; i++ ) {
		if( mBlockData[i] != NULL )
			delete mBlockData[i];
	}
}

void WorldChunk::initalize()
{
	for( size_t i = 0; i < CHUNK_SIZE; i++ ) {
		mBlockData[i] = NULL;
	}
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

	block->_setChunk( this );

	long k = BLOCK_INDEX( block );
	BlockPtr lb = mBlockData[ k ];

	if(lb != NULL) {
		delete lb;
	}
	mBlockData[ k ] = block;

	if(block->isThinking())
		mThinkingBlocks.insert( BlockList::value_type( k, block ) );

	mHasChanged = true;
	mHasGenerated = false;
}

void WorldChunk::_blockMoved( BaseBlock* block, short x, short y, short z )
{
	long k = BLOCK_INDEX( block );
	long kn = z * CHUNK_WIDTH * CHUNK_HEIGHT + y * CHUNK_WIDTH + x;
	BlockPtr lb = mBlockData[ k ];
	removeBlockAt( x, y, z );
	mBlockData[ k ] = block;
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
	BlockPtr it = mBlockData[ k ];
	if( it != NULL )  {
		if( it->isThinking() ) {
			BlockList::iterator thinker = mThinkingBlocks.find( k );
			if( thinker != mThinkingBlocks.end() )
				mThinkingBlocks.erase( thinker );
		}
		delete it;
		_blockVisible( it, false );
		mBlockData[ k ] = NULL;

	}
	mHasChanged = true;
	mHasGenerated = false;
}

void WorldChunk::_addBlockToRemoveList(BaseBlock* block)
{
	mShouldDelete.insert( BlockList::value_type( BLOCK_INDEX( block ) , block ) );
}

WorldChunk* WorldChunk::getRelativeChunk(unsigned short x, unsigned short y, unsigned short z)
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
	size_t k =  z * CHUNK_WIDTH * CHUNK_HEIGHT + y * CHUNK_WIDTH + x;
	if( k >= CHUNK_SIZE || k < 0) return NULL;
	BlockPtr it = mBlockData[k];
	return it;
}

BlockArray* WorldChunk::getBlocks()
{
	return &mBlockData;
}

size_t WorldChunk::getBlockCount()
{
	return 5;//mBlockData.size();
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
	for( size_t i = 0; i < CHUNK_SIZE; i++ ) {
		if( mBlockData[i] == NULL ) continue;
		BaseBlock* b = mBlockData[i];
		short visFlags = 0;
		short visOrig = b->getVisFlags();
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
		b->updateVisFlags(visFlags);
		for( size_t f = 0; f < 6; f++ ) {
			if( ((1<<f) & visOrig ) == (1<<f) ) {
				if( ( visOrig & visFlags ) != (1<<f) ) {
					b->connectedChange( (1<<f) );
				}
			}
		}
		if( visFlags == 0 )
			_blockVisible( b, false );
		else
			_blockVisible( b, true);
	}
}

void WorldChunk::_blockVisible( BlockPtr &block, bool v )
{
	BlockList::iterator it = mVisibleBlocks.find( BLOCK_INDEX( block ) );
	if( v && it == mVisibleBlocks.end() ) {
		mVisibleBlocks.insert( BlockList::value_type( BLOCK_INDEX( block ), block ) );
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

		for( BlockList::iterator block = mThinkingBlocks.begin(); block != mThinkingBlocks.end(); ++block )
		{
			if( block->second->isFluid() ) {
				((WaterBlock*)(block->second))->flow( 0.1f );
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
	if( mHasChanged || !mHasGenerated ) {
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
