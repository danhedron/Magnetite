#include "Chunk.h"
#include "BaseBlock.h"
#include "BlockFactory.h"
#include "LightingManager.h"

#include "MagnetiteCore.h"
#include "Renderer.h"

Chunk::Chunk( ChunkIndex index )
: mGeometry (NULL),
mPhysicsBody( NULL ),
mPhysicsMesh( NULL ),
mPhysicsShape( NULL ),
mPhysicsState( NULL )
{
	mVisibleFaces = 0;
	mWorldIndex = index;
	// Allocate block memory
	_allocateArray( CHUNK_SIZE );
	
	for(int x = 0; x < CHUNK_WIDTH; x++)
	{
		for(int z = 0; z < CHUNK_WIDTH; z++)
		{
			for(int y = 0; y < CHUNK_HEIGHT; y++)
			{
				BaseBlock* block = NULL;
				block = FactoryManager::getManager().createBlock("stone");
				setBlockAt( block, x, y, z  );
			}
		}
	}
}

Chunk::~Chunk()
{
	delete[] mBlocks;
}

long Chunk::getX()
{
	return mWorldIndex.x;
}

long Chunk::getY()
{
	return mWorldIndex.y;
}

long Chunk::getZ()
{
	return mWorldIndex.z;
}

size_t Chunk::getLightLevel( short x, short y, short z )
{
	return 100;
}

void Chunk::_allocateArray( size_t size )
{
	mBlocks = new BlockPtr[size];
	memset( mBlocks, 0, sizeof( BlockPtr ) * size );
}

BlockPtr* Chunk::getBlocks()
{
	return mBlocks;
}

void Chunk::setBlockAt( BlockPtr block, short x, short y, short z )
{
	setBlockAt( block, BLOCK_INDEX_2( x, y, z ) );
}

void Chunk::setBlockAt( BlockPtr block, short index )
{
	if( index < 0 || index > CHUNK_SIZE )
		return;
	mBlocks[ index ] = block;
	_raiseChunkFlag( DataUpdated );
}

BlockPtr Chunk::getBlockAt( short x, short y, short z )
{
	return getBlockAt( BLOCK_INDEX_2( x, y, z ) );
}

BlockPtr Chunk::getBlockAt( size_t index )
{
	if( index < 0 || index > CHUNK_SIZE - 1 )
		return NULL;
	return mBlocks[ index ];
}

void Chunk::removeBlockAt( short x, short y, short z )
{
	removeBlockAt( BLOCK_INDEX_2( x, y, z ) );
}

void Chunk::removeBlockAt( short index )
{
	if( index < 0 || index > CHUNK_SIZE )
		return;
	delete mBlocks[index];
	mBlocks[index] = NULL;
	_raiseChunkFlag( DataUpdated );
}

bool Chunk::hasNeighbours( short x, short y, short z )
{
	if( getBlockAt(x - 1, y, z) ) return true;
	if( getBlockAt(x + 1, y, z) ) return true;
	if( getBlockAt(x, y - 1, z) ) return true;
	if( getBlockAt(x, y + 1, z ) ) return true;
	if( getBlockAt(x, y, z - 1) ) return true;
	if( getBlockAt(x, y, z + 1) ) return true;
	return false;
}

void Chunk::updateVisibility( )
{
	if( _hasChunkFlag( DataUpdated ) )
	{
		for( size_t x = 0; x < CHUNK_WIDTH; x++ ) {
			for( size_t y = 0; y < CHUNK_HEIGHT; y++ ) {
				for( size_t z = 0; z < CHUNK_WIDTH; z++ ) {
					if( mBlocks[BLOCK_INDEX_2( x, y, z)] == NULL ) continue;
					BaseBlock* b = mBlocks[BLOCK_INDEX_2( x, y, z)];
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
					BlockList::iterator it = mVisibleBlocks.find( BLOCK_INDEX_2( x, y, z ) );
					if( visFlags == 0 && it != mVisibleBlocks.end() )
						mVisibleBlocks.erase( it );
					else if( visFlags != 0 && it == mVisibleBlocks.end() )
						mVisibleBlocks.insert( BlockList::value_type( BLOCK_INDEX_2( x, y, z ), b ) );
				}
			}
		}	
		_raiseChunkFlag( MeshInvalid );
	}
}

void Chunk::requestGenerate()
{
	if( _hasChunkFlag( DataUpdated ) )
	{
		updateVisibility();
		if( _hasChunkFlag( MeshInvalid ) )
		{
			generate();
			_lowerChunkFlag( MeshInvalid );
		}
		_lowerChunkFlag( DataUpdated );
	}
}

GLgeometry* Chunk::getGeometry()
{
	return mGeometry;
}

void Chunk::generate()
{
	if( mGeometry != NULL ) 
	{
		mGeometry->releaseBuffer();
		delete[] mGeometry->vertexData;
		delete[] mGeometry->edgeData;
	}
	else
	{
		mGeometry = new GLgeometry();
	}
	
	GLuint numVerts = mVisibleFaces * 4;
	GLuint numEdges = mVisibleFaces * 6;
	
	GLvertex*	verts = new GLvertex[numVerts];
	GLedge*	edges = new GLedge[numEdges];
	
	size_t vind = 0;
	size_t eind = 0;
	
	LightingManager::lightChunk( this );
	
	BlockContext context;
	
	for( BlockList::iterator it = mVisibleBlocks.begin(); it != mVisibleBlocks.end(); ++it )
	{
		Vector3 pos = Util::indexToPosition( it->first );
		context.worldX = pos.x;
		context.worldY = pos.y;
		context.worldZ = pos.z;
		context.chunk = this;
		it->second->buildCubeData( context, vind, eind, verts, edges );
	}
	
	mGeometry->edgeData	= edges;
	mGeometry->vertexData	= verts;
	mGeometry->edgeCount	= numEdges;
	mGeometry->vertexCount	= numVerts;
	
	// generate physics
	//generatePhysics();
}

void Chunk::generatePhysics()
{
	if( mPhysicsBody != NULL )
	{
		CoreSingleton->getPhysicsWorld()
			->removeRigidBody( mPhysicsBody );
		delete mPhysicsState;
		delete mPhysicsBody;
		delete mPhysicsShape;
		delete mPhysicsMesh;
	}
	
	if( mGeometry->vertexCount > 0 && mGeometry->edgeData > 0 )
	{
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
		CoreSingleton->getPhysicsWorld()
			->addRigidBody( mPhysicsBody );
	}
}

void Chunk::update( float dt )
{
	requestGenerate();
}

void Chunk::_raiseChunkFlag( uint16_t flag )
{
	mChunkFlags |= flag;
}

bool Chunk::_hasChunkFlag( uint16_t flag )
{
	return ( mChunkFlags & flag == flag );
}

void Chunk::_lowerChunkFlag( uint16_t flag )
{
	mChunkFlags &= (~flag);
}
