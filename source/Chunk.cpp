
#include "Chunk.h"
#include "BaseBlock.h"
#include "BlockFactory.h"
#include "LightingManager.h"

#include "MagnetiteCore.h"
#include "World.h"
#include "Renderer.h"
#include <BaseTriangulator.h>
#include <util.h>

#include "Geometry.h"

Chunk::Chunk( ChunkIndex index, World* world )
: mWorld( world ),
mGeometry (NULL),
mChunkFlags( 0 ),
mPhysicsShape( NULL ),
mPhysicsState( NULL ),
mPhysicsMesh( NULL ),
mPhysicsBody( NULL ),
mNumBlocks( 0 )
{
	mVisibleFaces = 0;
	mWorldIndex = index;
	
	// Allocates the internal block array 
	_allocateArray( CHUNK_SIZE );
}

Chunk::~Chunk()
{
	delete[] mBlocks;
	delete[] mLightValues;
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

World* Chunk::getWorld()
{
	return mWorld;
}

size_t Chunk::getLightLevel( short x, short y, short z )
{
	if( x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_WIDTH )
		return 255;
	return mLightValues[ BLOCK_INDEX_2( x, y, z ) ];
}

void Chunk::_allocateArray( size_t size )
{
	mBlocks = new BlockPtr[size];
	memset( mBlocks, 0, sizeof( BlockPtr ) * size );
	
	mLightValues = new LightIndex[size];
	memset( mBlocks, 0, sizeof( LightIndex ) * size );
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

BlockPtr Chunk::getBlockAt( short x, short y, short z )
{
	if( x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_WIDTH )
		return NULL;
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
	getMutex().lock();
	
	if( index < 0 || index > CHUNK_SIZE )
		return;
	BlockList::iterator it = mVisibleBlocks.find( index );
	if( it != mVisibleBlocks.end() )
		mVisibleBlocks.erase( it );
	delete mBlocks[index];
	mNumBlocks--;
	mBlocks[index] = NULL;
	_raiseChunkFlag( DataUpdated );
	
	getMutex().unlock();
}

bool Chunk::hasNeighbours( long x, long y, long z )
{
	if( x == 0 || y == 0 || z == 0 ) return true;
	if( x >= CHUNK_WIDTH-1 ||  y >= CHUNK_HEIGHT-1 ||  z >= CHUNK_WIDTH-1 ) return true;
	if( getBlockAt(x - 1, y, z) ) return true;
	if( getBlockAt(x + 1, y, z) ) return true;
	if( getBlockAt(x, y - 1, z) ) return true;
	if( getBlockAt(x, y + 1, z ) ) return true;
	if( getBlockAt(x, y, z - 1) ) return true;
	if( getBlockAt(x, y, z + 1) ) return true;
	return false;
}

void Chunk::setLightLevel( LightIndex value, short x, short y, short z )
{
	if( x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_HEIGHT || z < 0 || z >= CHUNK_WIDTH )
		return;
	mLightValues[ BLOCK_INDEX_2( x, y, z ) ] = value;
}

size_t Chunk::getVisibleFaceCount()
{
    return mVisibleFaces;
}

BlockList& Chunk::getVisibleBlocks()
{
		return mVisibleBlocks;
}

void Chunk::updateVisibility( )
{
	MagnetiteCore* core = CoreSingleton;
	World* w = core->getWorld();
	BaseBlock* b = nullptr;
	short visFlags = 0;
	
	if( _hasChunkFlag( DataUpdated ) )
	{
		mVisibleFaces = 0;
		long worldX = getX() * CHUNK_WIDTH;
		long worldY = getY() * CHUNK_HEIGHT;
		long worldZ = getZ() * CHUNK_WIDTH;
		for( long x = 0; x < CHUNK_WIDTH; x++ ) {
			for( long y = 0; y < CHUNK_HEIGHT; y++ ) {
				for( long z = 0; z < CHUNK_WIDTH; z++ ) {
					auto bindex = BLOCK_INDEX_2( x, y, z);
					b = mBlocks[bindex];
					if( b == nullptr ) continue;
					visFlags = 0;
					//Check All axes for adjacent blocks.
					BaseBlock* cb = w->getBlockAt( worldX + x + 1, worldY + y, worldZ + z );
					if( cb == NULL || !cb->isOpaque() ) {
						mVisibleFaces++;
						visFlags = visFlags | FACE_RIGHT;
					}
					cb = w->getBlockAt( worldX + x - 1, worldY + y, worldZ + z );
					if( worldX + x != 0 && (cb == NULL || !cb->isOpaque()) ) {
						mVisibleFaces++;
						visFlags = visFlags | FACE_LEFT;
					}
					cb = w->getBlockAt( worldX + x, worldY + y + 1, worldZ + z );
					if( cb == NULL || !cb->isOpaque() ) {
						mVisibleFaces++;
						visFlags = visFlags | FACE_TOP;
					}
					cb = w->getBlockAt( worldX + x, worldY + y - 1, worldZ + z );
					if( worldY + y != 0 && (cb == NULL || !cb->isOpaque()) ) {
						mVisibleFaces++;
						visFlags = visFlags | FACE_BOTTOM;
					}
					cb = w->getBlockAt( worldX + x, worldY + y, worldZ + z + 1 );
					if( cb == NULL || !cb->isOpaque() ) {
						mVisibleFaces++;
						visFlags = visFlags | FACE_BACK;
					}
					cb = w->getBlockAt( worldX + x, worldY + y, worldZ + z - 1 );
					if( worldZ + z != 0 && (cb == NULL || !cb->isOpaque()) ) {
						mVisibleFaces++;
						visFlags = visFlags | FACE_FORWARD;
					}
					b->updateVisFlags(visFlags);
					BlockList::iterator it = mVisibleBlocks.find( bindex );
					if( visFlags == 0 && it != mVisibleBlocks.end() )
						mVisibleBlocks.erase( it );
					else if( visFlags != 0 && it == mVisibleBlocks.end() )
						mVisibleBlocks.insert( BlockList::value_type( bindex, b ) );
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
		Perf::Profiler::get().begin("vupdate");
		updateVisibility();
		Perf::Profiler::get().end("vupdate");
		if( _hasChunkFlag( MeshInvalid ) )
		{
			generate();
			_lowerChunkFlag( MeshInvalid );
		}
		_lowerChunkFlag( DataUpdated );
	}
}

GeometryPtr Chunk::getGeometry()
{
	return mGeometry;
}

void Chunk::generate()
{
	// Lighting must be done before geometry
	Perf::Profiler::get().begin("lupdate");
	generateLighting();
	Perf::Profiler::get().end("lupdate");
	
	Perf::Profiler::get().begin("cgupdate");
	generateGeometry();
	Perf::Profiler::get().end("cgupdate");
	
	Perf::Profiler::get().begin("pupdate");
	// generate physics - geometry must already be generated
	generatePhysics();
	Perf::Profiler::get().end("pupdate");
}

void Chunk::generateGeometry()
{
	if( mGeometry == NULL ) 
	{
		mGeometry = new TerrainGeometry();
	}
	else
	{
		delete mGeometry;
		mGeometry = new TerrainGeometry();
	}
	
	if( mVisibleBlocks.size() > 0 )
	{
		mWorld->getTriangulator()->triangulateChunk(mGeometry, this);
	}
}

void Chunk::generateLighting()
{
	LightingManager::lightChunk( this );
}

void Chunk::generatePhysics()
{
	if( mPhysicsBody != NULL )
	{
		auto p = mPhysicsBody;
		mPhysicsBody = nullptr;
		CoreSingleton->physicsMutex.lock();
		CoreSingleton->getPhysicsWorld()
			->removeRigidBody( p );
		CoreSingleton->physicsMutex.unlock();
		delete mPhysicsState;
		delete mPhysicsBody;
		delete mPhysicsShape;
		delete mPhysicsMesh;
	}
	
	if( mGeometry->vertexCount > 0 && mGeometry->edgeData > 0 )
	{
		btTriangleIndexVertexArray* meshInterface = new btTriangleIndexVertexArray;
		btIndexedMesh part;

		int vertSize = sizeof( TerrainVertex );
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
		
		CoreSingleton->physicsMutex.lock();
		CoreSingleton->getPhysicsWorld()
			->addRigidBody( mPhysicsBody );
		CoreSingleton->physicsMutex.unlock();
	}
}

void Chunk::update( float dt )
{
	/*BlockContext context;
	for( size_t i = 0; i < CHUNK_SIZE; i += 1 )
	{
		if( mBlocks[i] )
		{
			Vector3 pos = Util::indexToPosition( i );
			context.worldX = pos.x;
			context.worldY = pos.y;
			context.worldZ = pos.z;
			context.chunk = this;
			context.world = MagnetiteCore::Singleton->getWorld();
			mBlocks[i]->think( context, dt );
		}
	}*/
	
	requestGenerate();
}

void Chunk::_raiseChunkFlag( uint16_t flag )
{
	mChunkFlags |= flag;
}

bool Chunk::_hasChunkFlag( uint16_t flag )
{
	return ( (mChunkFlags & flag) == flag );
}

void Chunk::_lowerChunkFlag( uint16_t flag )
{
	mChunkFlags &= (~flag);
}

const size_t Chunk::getBlockCount()
{
	return mNumBlocks;
}

std::mutex& Chunk::getMutex()
{
	return mMutex;
}