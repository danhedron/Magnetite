
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


BaseBlock* Chunk::getBlockAtWorld( ChunkScalar x, ChunkScalar y, ChunkScalar z )
{
	if( x < 0 || y < 0 || z < 0 || x / CHUNK_WIDTH != mWorldIndex.x || y / CHUNK_HEIGHT != mWorldIndex.y || z / CHUNK_WIDTH != mWorldIndex.z ) return mWorld->getBlockAt(x,y,z);
	auto id = (z%CHUNK_WIDTH) * CHUNK_WIDTH * CHUNK_HEIGHT + (y%CHUNK_HEIGHT) * CHUNK_WIDTH + (x%CHUNK_WIDTH);
	return mBlocks[id];
}

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
	for( size_t i = 0; i < CHUNK_SIZE; i ++ )
	{
		if( mBlocks[i] != nullptr )
			delete mBlocks[i];
	}
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

void Chunk::removeBlockAt( short x, short y, short z )
{
	removeBlockAt( BLOCK_INDEX_2( x, y, z ) );
}

void Chunk::removeBlockAt( short index )
{
	if( index < 0 || index > CHUNK_SIZE )
		return;
	BlockList::iterator it = mVisibleBlocks.find( index );
	if( it != mVisibleBlocks.end() )
		mVisibleBlocks.erase( it );
	delete mBlocks[index];
	mNumBlocks--;
	mBlocks[index] = NULL;
	_raiseChunkFlag( DataUpdated );
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

size_t Chunk::getVisibleFaceCount()
{
    return mVisibleFaces;
}

BlockList& Chunk::getVisibleBlocks()
{
	return mVisibleBlocks;
}

#define CHECK_FACE( cond, dx, dy, dz, face ) \
	if( cond || mWorld->getChunk( getX() + dx, getY() + dy, getZ() + dz ) != nullptr ) \
	{\
		cb = getBlockAtWorld( blockX + dx, blockY + dy, blockZ + dz );\
		if( cb == NULL || !cb->isOpaque() ) {\
			mVisibleFaces++;\
			visFlags |= face;\
		}\
	}

void Chunk::updateVisibility( )
{
	BaseBlock* b = nullptr;
	BaseBlock* cb = nullptr;
	short visFlags = 0;
	BlockList::iterator it;
	
	if( _hasChunkFlag( DataUpdated ) && getBlockCount() > 0 )
	{
		size_t id = 0;
		mVisibleFaces = 0;
		ChunkScalar worldX = getX() * CHUNK_WIDTH;
		ChunkScalar worldY = getY() * CHUNK_HEIGHT;
		ChunkScalar worldZ = getZ() * CHUNK_WIDTH;
		ChunkScalar blockX = 0;
		ChunkScalar blockY = 0;
		ChunkScalar blockZ = 0;
		
		bool fc = getBlockCount() == CHUNK_SIZE;
		
		for( long z = 0; z < CHUNK_WIDTH; z++ ) {
			for( long y = 0; y < CHUNK_HEIGHT; y++ ) {
				for( long x = 0; x < CHUNK_WIDTH; x++ ) {
					id = BLOCK_INDEX_2( x, y, z );
					b = mBlocks[id];
					if( b == nullptr ) { continue; }
					if( fc && ( x > 0 && x < CHUNK_WIDTH-1 && y > 0 && y < CHUNK_HEIGHT-1 && z > 0 && z < CHUNK_WIDTH-1 ) ) { continue; }
				
					blockX = worldX + x; blockY = worldY + y; blockZ = worldZ + z;
					
					visFlags = 0;
					//Check All axes for adjacent blocks.
					CHECK_FACE( x < CHUNK_WIDTH-1,  1, 0, 0, FACE_RIGHT );
					
					CHECK_FACE( x > 0,             -1, 0, 0, FACE_LEFT );
					
					CHECK_FACE( y < CHUNK_HEIGHT-1, 0, 1, 0, FACE_TOP );
					
					CHECK_FACE( y > 0,              0,-1, 0, FACE_BOTTOM );
					
					CHECK_FACE( z < CHUNK_WIDTH-1,  0, 0, 1, FACE_BACK );
					
					CHECK_FACE( z > 0,              0, 0,-1, FACE_FORWARD );
					
					if( b->getVisFlags() != visFlags ) {
						// Check to see if we need to change the VB data.
						if( visFlags > 0 && b->getVisFlags() == 0 ) {
							mVisibleBlocks.insert( BlockList::value_type( id, b ) );
						}
						else if( visFlags == 0 && b->getVisFlags() > 0 )
						{
							it = mVisibleBlocks.find( id );
							mVisibleBlocks.erase( it );
						}
						b->updateVisFlags(visFlags);
					}
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
	if( !_hasChunkFlag( SkipLight ) )
	{
		LightingManager::lightChunk( this );
	}
	else
	{
		_lowerChunkFlag( SkipLight );
	}
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
		if( getBlockCount() < CHUNK_SIZE )
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

			//mPhysicsShape = new btBvhTriangleMeshShape( meshInterface, true );//new btConvexTriangleMeshShape( meshInterface );////new btBoxShape( btVector3(8, 64, 8) );
			mPhysicsShape = new btBvhTriangleMeshShape( meshInterface, false );
			mPhysicsState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3( getX() * CHUNK_WIDTH, getY() * CHUNK_HEIGHT, getZ() * CHUNK_WIDTH)));
			btRigidBody::btRigidBodyConstructionInfo ci( 0, mPhysicsState, mPhysicsShape, btVector3(0,0,0) );
			mPhysicsBody = new btRigidBody( ci );
			
			mPhysicsBody->setCollisionFlags( mPhysicsBody->getCollisionFlags() | btRigidBody::CF_STATIC_OBJECT );
		}
		else
		{
			// Chunk is a solid block.
			mPhysicsShape = new btBoxShape( btVector3(CHUNK_WIDTH/2, CHUNK_HEIGHT/2, CHUNK_WIDTH/2) );
			mPhysicsState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3( getX() * CHUNK_WIDTH, getY() * CHUNK_HEIGHT, getZ() * CHUNK_WIDTH)));
			btRigidBody::btRigidBodyConstructionInfo ci( 0, mPhysicsState, mPhysicsShape, btVector3(0,0,0) );
			mPhysicsBody = new btRigidBody( ci );
			mPhysicsBody->setCollisionFlags( mPhysicsBody->getCollisionFlags() | btRigidBody::CF_STATIC_OBJECT );
		}
		
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
