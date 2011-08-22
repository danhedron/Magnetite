#include "WorldChunk.h"
#include "BaseBlock.h"
#include "WaterBlock.h"
#include "BlockFactory.h"
#include "Renderer.h"
#include "OpencraftCore.h"
#include "World.h"
#include "LightingManager.h"

WorldChunk::WorldChunk(long x, long y, long z)
: mX( x ),
mY( y ),
mZ( z ),
mHasChanged( false ),
mHasGenerated( false ),
mGeometry( NULL ),
mVisibleFaces( 0 ),
mUpdateTimer( 0 ),
mPhysicsShape( NULL ),
mPhysicsBody( NULL ),
mPhysicsMesh( NULL ),
mPhysicsState( NULL )
{
	mBlockData = new BlockPtr[CHUNK_SIZE];
	initalize();
}

WorldChunk::~WorldChunk(void)
{
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

void WorldChunk::setLightLevel( short x, short y, short z, LightIndex level )
{
	mLightValues[BLOCK_INDEX_2( x, y, z )] = level;
	//if( isEdge( x, y, z ) )
		//updateSurrounding();
}

LightIndex WorldChunk::getLightLevel( short x, short y, short z )
{
	WorldChunk* chunk = getRelativeChunk( x, y, z );
	if( chunk == this ) {
		return mLightValues[BLOCK_INDEX_2( x, y, z )];
	}
	else if( chunk != NULL) {
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
		return chunk->getLightLevel( x, y, z );
	}
		
	return Sunlight;
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
						block = FactoryManager::getManager().createBlock("grass");
					else if( y == 8 || y == 7 )
						block = FactoryManager::getManager().createBlock("dirt");
					else
						block = FactoryManager::getManager().createBlock("stone");
					addBlockToChunk( block, x, y, z  );
				}
			}
		}
	}
}

void WorldChunk::addBlockToChunk(BaseBlock* block, short x, short y, short z)
{
	WorldChunk* chunk = getRelativeChunk( x, y, z );
	if( chunk != this ) {
		if( chunk == NULL ) {
			delete block;
			return;
		}
		if( x < 0 )
			x = x + CHUNK_WIDTH;
		else if( x >= CHUNK_WIDTH )
			x = x - CHUNK_WIDTH;
		if( y < 0 )
			y = y + CHUNK_HEIGHT;
		else if( y >= CHUNK_HEIGHT )
			y = y - CHUNK_HEIGHT;
		if( z < 0 )
			z = z + CHUNK_WIDTH;
		else if( z >= CHUNK_WIDTH )
			z = z - CHUNK_WIDTH;
		chunk->addBlockToChunk( block, x, y, z );
		return;
	}
	
	long k = BLOCK_INDEX_2( x, y, z );
	BlockPtr lb = mBlockData[ k ];

	if(lb != NULL) {
		delete lb;
	}
	mBlockData[ k ] = block;
	if( isEdge( x, y, z ) ) 
		updateSurrounding();

	if(block->isThinking())
		mThinkingBlocks.insert( BlockList::value_type( k, block ) );

	mHasChanged = true;
	mHasGenerated = false;
}

void WorldChunk::_blockMoved( BaseBlock* block, short x, short y, short z )
{
	long k = BLOCK_INDEX_2( x, y, z );
	long kn = z * CHUNK_WIDTH * CHUNK_HEIGHT + y * CHUNK_WIDTH + x;
	BlockPtr lb = mBlockData[ k ];
	removeBlockAt( x, y, z );
	if( isEdge( x, y, z ) ) 
		updateSurrounding();
	mBlockData[ k ] = block;
	markModified();
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
		if( isEdge( x, y, z ) ) 
			updateSurrounding();
		delete it;
		_blockVisible( k, false );
		mBlockData[ k ] = NULL;

	}
	mHasChanged = true;
	mHasGenerated = false;
}

void WorldChunk::removeBlockAt( long index )
{
	long k = index;
	BlockPtr it = mBlockData[ k ];
	if( it != NULL )  {
		if( it->isThinking() ) {
			BlockList::iterator thinker = mThinkingBlocks.find( k );
			if( thinker != mThinkingBlocks.end() )
				mThinkingBlocks.erase( thinker );
		}
		delete it;
		_blockVisible( k, false );
		mBlockData[ k ] = NULL;
	}
	mHasChanged = true;
	mHasGenerated = false;
}

void WorldChunk::_addBlockToRemoveList(BaseBlock* block)
{
	//mShouldDelete.insert( BlockList::value_type( BLOCK_INDEX( block ) , block ) );
}

WorldChunk* WorldChunk::getRelativeChunk(short x, short y, short z)
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
	return OpencraftCore::Singleton->getWorld()->getChunk( mX + std::floorl( (float)x/CHUNK_WIDTH ) , mY + std::floorl( (float)y/CHUNK_HEIGHT ) , mZ + std::floorl( (float)z/CHUNK_WIDTH ) );
	//return chunk;
}

void WorldChunk::updateSurrounding( )
{
	if( OpencraftCore::Singleton->getWorld()->getCurrentStage() != WORLD_GEN ) {
		WorldChunk* c = NULL;
		c = getRelativeChunk( -1, 0, 0 );
		if( c ) c->markModified();
		c = getRelativeChunk( 16, 0, 0 );
		if( c ) c->markModified();
		c = getRelativeChunk( 0, -1, 0 );
		if( c ) c->markModified();
		c = getRelativeChunk( 0, 16, 0 );
		if( c ) c->markModified();
		c = getRelativeChunk( 0, 0, -1 );
		if( c ) c->markModified();
		c = getRelativeChunk( 0, 0, 16 );
		if( c ) c->markModified();
	}
}

bool WorldChunk::hasNeighbours( short x, short y, short z )
{
	WorldChunk* chunk = getRelativeChunk( x, y, z );
	if( chunk != this && chunk != NULL ) {
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
		return chunk->hasNeighbours( x, y, z );
	}

	if( getBlockAt(x - 1, y, z) ) return true;
	if( getBlockAt(x + 1, y, z) ) return true;
	if( getBlockAt(x, y - 1, z) ) return true;
	if( getBlockAt(x, y + 1, z ) ) return true;
	if( getBlockAt(x, y, z - 1) ) return true;
	if( getBlockAt(x, y, z + 1) ) return true;
	return false;
}

bool WorldChunk::isEdge( short x, short y, short z )
{
	if( x <= 0 || y <= 0 || z <= 0 ) return true;
	if( x >= CHUNK_WIDTH-1 || y >= CHUNK_HEIGHT -1|| z >= CHUNK_WIDTH-1 ) return true;
	return false;
}

BaseBlock* WorldChunk::getBlockAt(long x, long y, long z)
{
	// For a 3D array stored in 1D array, you must:-
	//  z*height*width + y * width + x
	WorldChunk* chunk = getRelativeChunk( x, y, z );
	if( chunk != this ) {
		// If the coordinates fall outside of this chunk, silently return the chunk containing that relative block.
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
		if( chunk ) {
			return chunk->getBlockAt(x, y, z);
		}
		else {
			return NULL;
		}
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
	for( size_t x = 0; x < CHUNK_WIDTH; x++ ) {
		for( size_t y = 0; y < CHUNK_HEIGHT; y++ ) {
			for( size_t z = 0; z < CHUNK_WIDTH; z++ ) {
				if( mBlockData[BLOCK_INDEX_2( x, y, z)] == NULL ) continue;
				BaseBlock* b = mBlockData[BLOCK_INDEX_2( x, y, z)];
				short visFlags = 0;
				short visOrig = b->getVisFlags();
				//Check All axes for adjacent blocks.
				BaseBlock* cb = getBlockAt( x + 1, y, z );
				if( cb == NULL || !cb->isOpaque() ) {
					mVisibleFaces++;
					visFlags = visFlags | FACE_RIGHT;
				}
				cb = getBlockAt( x - 1, y, z );
				if( cb == NULL || !cb->isOpaque() ) {
					mVisibleFaces++;
					visFlags = visFlags | FACE_LEFT;
				}
				cb = getBlockAt( x, y + 1, z );
				if( cb == NULL || !cb->isOpaque() ) {
					mVisibleFaces++;
					visFlags = visFlags | FACE_TOP;
				}
				cb = getBlockAt( x, y - 1, z );
				if( cb == NULL || !cb->isOpaque() ) {
					mVisibleFaces++;
					visFlags = visFlags | FACE_BOTTOM;
				}
				cb = getBlockAt( x, y, z + 1 );
				if( cb == NULL || !cb->isOpaque() ) {
					mVisibleFaces++;
					visFlags = visFlags | FACE_BACK;
				}
				cb = getBlockAt( x, y, z - 1 );
				if( cb == NULL || !cb->isOpaque() ) {
					mVisibleFaces++;
					visFlags = visFlags | FACE_FORWARD;
				}
				b->updateVisFlags(visFlags);
				/*for( size_t f = 0; f < 6; f++ ) {
					if( ((1<<f) & visOrig ) == (1<<f) ) {
						if( ( visOrig & visFlags ) != (1<<f) ) {
							b->connectedChange( (1<<f) );
						}
					}
				}*/
				if( visFlags == 0 )
					_blockVisible( BLOCK_INDEX_2( x, y, z), false );
				else
					_blockVisible( BLOCK_INDEX_2( x, y, z), true);
			}
		}
	}
}

void WorldChunk::_blockVisible( size_t index, bool v )
{
	BlockList::iterator it = mVisibleBlocks.find( index );
	if( v && it == mVisibleBlocks.end() ) {
		mVisibleBlocks.insert( BlockList::value_type( index, mBlockData[index] ) );
	}
	else if( !v && it != mVisibleBlocks.end() )  {
		mVisibleBlocks.erase( it++ );
	}
}

void WorldChunk::generate()
{
	if( mGeometry != NULL ) {
		mGeometry->releaseBuffer();
		delete[] mGeometry->vertexData;
		delete[] mGeometry->edgeData;
	}
	GLuint vertexCount	 = getVisibleFaceCount() * 4;
	GLuint edgeCount	 = getVisibleFaceCount() * 6;
	GLvertex* vertexData = new GLvertex[vertexCount];
	GLedge* edgeData	 = new GLedge[edgeCount];

	size_t ind = 0;
	size_t edgeInd = 0;

	// Re-light this chunk
	LightingManager::lightChunk( this );
	BlockContext bContext;

	for( BlockList::iterator block = mVisibleBlocks.begin(); block != mVisibleBlocks.end(); ++block )
	{
		Vector3 pos = Util::indexToPosition( block->first );
		bContext.worldX = pos.x;
		bContext.worldY = pos.y;
		bContext.worldZ = pos.z;
		bContext.chunk = this;
		block->second->buildCubeData(bContext, ind, edgeInd, vertexData, edgeData);
	}
	
	// If there's already a geometry object; recycle it.
	if( mGeometry == NULL )
		mGeometry = new GLgeometry;
	mGeometry->edgeData = edgeData;
	mGeometry->vertexData = vertexData;
	mGeometry->edgeCount = edgeCount;
	mGeometry->vertexCount = vertexCount;
	//mGeometry->bindToBuffer();

	generatePhysics();

	notifyGenerated();
	// Delete the chunk's previous data
	//mWorldBuffers.insert( ChunkGeomList::value_type( chunk, geom ) );
}

void WorldChunk::generatePhysics()
{
	if( mPhysicsBody != NULL ) {
		OpencraftCore::Singleton->getPhysicsWorld()->removeRigidBody( mPhysicsBody );
		delete mPhysicsState;
		delete mPhysicsBody;
		delete mPhysicsShape;
		delete mPhysicsMesh;
	}

	// Only re-build physics mesh if there is actually any mesh data.
	if( mGeometry->vertexCount > 0 && mGeometry->edgeCount > 0 ) {
		btTriangleIndexVertexArray* meshInterface = new btTriangleIndexVertexArray;
		btIndexedMesh part;

		int vertSize = sizeof( GLvertex );
		int indexSize = sizeof( GLedge );

		part.m_vertexBase = (const unsigned char*)&mGeometry->vertexData[0].x;
		part.m_vertexStride = vertSize;
		part.m_numVertices = mGeometry->vertexCount;
		part.m_vertexType = PHY_FLOAT;
		part.m_triangleIndexBase = (const unsigned char*)&mGeometry->edgeData[0];
		part.m_triangleIndexStride = indexSize * 3;
		part.m_numTriangles = mGeometry->edgeCount / 3;
		part.m_indexType = PHY_SHORT;

		meshInterface->addIndexedMesh( part, PHY_SHORT );

		mPhysicsShape = new btBvhTriangleMeshShape( meshInterface, true );//new btConvexTriangleMeshShape( meshInterface );////new btBoxShape( btVector3(8, 64, 8) );
		mPhysicsState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3( getX() * CHUNK_WIDTH, getY() * CHUNK_HEIGHT, getZ() * CHUNK_WIDTH)));
		btRigidBody::btRigidBodyConstructionInfo ci( 0, mPhysicsState, mPhysicsShape, btVector3(0,0,0) );
		mPhysicsBody = new btRigidBody( ci );
		mPhysicsBody->setCollisionFlags( mPhysicsBody->getCollisionFlags() | btRigidBody::CF_STATIC_OBJECT );
		OpencraftCore::Singleton->getPhysicsWorld()->addRigidBody( mPhysicsBody );
	}
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

void WorldChunk::forceGenerate()
{
	mHasGenerated = false;
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
				((WaterBlock*)(block->second))->flow( dt );
			}
		}
	}
	mUpdateTimer += dt;

	for( BlockList::iterator it = mShouldDelete.begin(); it != mShouldDelete.end(); ) {
		removeBlockAt( it->first );
		mShouldDelete.erase( it++ );
	}
}

void WorldChunk::requestGenerate()
{
	if( mHasChanged || !mHasGenerated ) {
		updateVisibility();
		if( !mHasGenerated ) {
			generate();
		}
		mHasChanged = false;
	}
}

void WorldChunk::appendToStream( std::ofstream& stream )
{
	size_t exportedBlocks = 0;
	for( size_t b = 0; b < CHUNK_SIZE; b++ ) {
		if( mBlockData[b] != 0 ) {
			exportedBlocks++;
		}
	}

	stream.write( (char *) &exportedBlocks, sizeof(exportedBlocks) );

	for( size_t b = 0; b < CHUNK_SIZE; b++ ) {
		if( mBlockData[b] != 0 ) {
			std::string type = mBlockData[b]->getType();
			stream.write( (char *) type.c_str(), type.length() );
			char null = 0;
			stream.write( (char *) &null, sizeof(null) );
			mBlockData[b]->appendToStream( stream );
		}
	}
}

void WorldChunk::readFromStream( std::ifstream& stream )
{
	size_t blocksToRead = 0;

	stream.read( (char *) &blocksToRead, sizeof(blocksToRead) );

	std::string type = "";
	char c = 0;

	for( size_t b = 0; b < blocksToRead; b++ ) {
		c = 0; type = "";
		while( stream.read( &c, sizeof(c) ) && !stream.eof() ) {
			if( (c < 32 || c > 126 ) && c != 20 ) break;
			type += c;
		}

		//Try to create that block.
		BaseBlock* block = FactoryManager::getManager().createBlock(type);
		if( block ) {
			block->readFromStream( stream );
			//addBlockToChunk( block );
		}
		else
		{
			Util::log( "Couldn't load block: " + type );
		}
		
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
