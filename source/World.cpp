#include "World.h"
#include "Chunk.h"
#include "BaseBlock.h"
#include "Renderer.h"
#include "Sky.h"
#include "Camera.h"
#include "ChunkGenerator.h"
#include "MagnetiteCore.h"
#include <LightingManager.h>
#include <BlockTriangulator.h>
#include <BaseEntity.h>
#include <WorldSerializer.h>
#include <Profiler.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <math.h>

#ifdef WIN32
#include <direct.h>
#include <ppl.h>
#else
#include <unistd.h>
#endif

// For convenience
#define c2i( x, y, z ) ( z * mWorldSize * mWorldSize + y * mWorldSize + x )

World::World( size_t edgeSize )
: mSky( NULL ),
mTriangulator( new BlockTriangulator() )
{	
	mWorldSize = edgeSize;
	mRegions = new Magnetite::ChunkRegionPtr[edgeSize*edgeSize*edgeSize];
	memset( mRegions, 0, sizeof( Magnetite::ChunkRegionPtr ) * edgeSize*edgeSize*edgeSize);
	
	setWorldSize( edgeSize * REGION_SIZE, edgeSize * REGION_SIZE, edgeSize * REGION_SIZE);
	
	setPageOffset( Vector3( CHUNK_WIDTH / 2.f, CHUNK_WIDTH / 2.f, CHUNK_WIDTH / 2.f ) );
	
	setPageSize( CHUNK_WIDTH );
	
	createSky( 0 );

	printDbg = false;
	
	mSerializer = new Magnetite::WorldSerializer(this);
}

World::~World()
{
	destoryWorld();
	
	delete mSerializer;
}

Magnetite::String World::getName()
{
	return mWorldName;
}

void World::setName( const Magnetite::String& name )
{
	mWorldName = name;
}

BaseTriangulator* World::getTriangulator()
{
	return mTriangulator;
}

BlockPtr World::getBlockAt( long x, long y, long z )
{
	ChunkScalar rx = x / REGION_WORLD_SIZE;
	ChunkScalar ry = y / REGION_WORLD_SIZE;
	ChunkScalar rz = z / REGION_WORLD_SIZE;
	Magnetite::ChunkRegionPtr r = getRegion(rx, ry, rz);
	if( r == NULL ) return NULL;
	ChunkScalar cx = (x / CHUNK_WIDTH) % REGION_SIZE;
	ChunkScalar cy = (y / CHUNK_WIDTH) % REGION_SIZE;
	ChunkScalar cz = (z / CHUNK_WIDTH) % REGION_SIZE;
	
	auto c = r->get( cx, cy, cz );
	
	if( c == NULL ) return NULL;
	
	return c->getBlockAt( x % CHUNK_WIDTH, y % CHUNK_HEIGHT, z % CHUNK_WIDTH );
}

void World::removeBlockAt( long x, long y, long z )
{
	ChunkScalar rx = x / REGION_WORLD_SIZE;
	ChunkScalar ry = y / REGION_WORLD_SIZE;
	ChunkScalar rz = z / REGION_WORLD_SIZE;
	Magnetite::ChunkRegionPtr r = getRegion(rx, ry, rz);
	if( r == NULL ) return;
	ChunkScalar cx = (x / CHUNK_WIDTH) % REGION_SIZE;
	ChunkScalar cy = (y / CHUNK_WIDTH) % REGION_SIZE;
	ChunkScalar cz = (z / CHUNK_WIDTH) % REGION_SIZE;
	
	auto c = r->get( cx, cy, cz );
	
	if( c == NULL ) return;
	
	c->removeBlockAt( x % CHUNK_WIDTH, y % CHUNK_HEIGHT, z % CHUNK_WIDTH );
}

void World::setBlockAt( BaseBlock* b, long x, long y, long z )
{
	ChunkScalar rx = x / REGION_WORLD_SIZE;
	ChunkScalar ry = y / REGION_WORLD_SIZE;
	ChunkScalar rz = z / REGION_WORLD_SIZE;
	Magnetite::ChunkRegionPtr r = getRegion(rx, ry, rz);
	if( r == NULL ) return;
	ChunkScalar cx = (x / CHUNK_WIDTH) % REGION_SIZE;
	ChunkScalar cy = (y / CHUNK_WIDTH) % REGION_SIZE;
	ChunkScalar cz = (z / CHUNK_WIDTH) % REGION_SIZE;
	
	auto c = r->get( cx, cy, cz );
	if( c == NULL ) c = r->create( cx, cy, cz );
	
	c->setBlockAt( b, x % CHUNK_WIDTH, y % CHUNK_HEIGHT, z % CHUNK_WIDTH );
}

void World::moveBlock( long x, long y, long z, float time, long ex, long ey, long ez )
{
	BaseBlock* block = getBlockAt( x, y, z );
	if( block )
	{
		MovingBlock mv;
		mv.block = block;
		mv.geom = NULL;
		mv.start = Vector3( x, y, z );
		mv.current = mv.start;
		mv.end = Vector3( ex, ey, ez );
		mv.time = time;
		mMovingBlocks.push_back(mv);
	}
}

MovingBlockList& World::getMovingBlocks()
{
	return mMovingBlocks;
}

LightIndex World::getLightLevel( long x, long y, long z )
{
	ChunkScalar rx = x / REGION_WORLD_SIZE;
	ChunkScalar ry = y / REGION_WORLD_SIZE;
	ChunkScalar rz = z / REGION_WORLD_SIZE;
	Magnetite::ChunkRegionPtr r = getRegion(rx, ry, rz);
	if( r == NULL ) return 255;
	ChunkScalar cx = (x / CHUNK_WIDTH) % REGION_SIZE;
	ChunkScalar cy = (y / CHUNK_WIDTH) % REGION_SIZE;
	ChunkScalar cz = (z / CHUNK_WIDTH) % REGION_SIZE;
	
	auto c = r->get( cx, cy, cz );
	if( c == NULL ) return 255;
	return c->getLightLevel( x % CHUNK_WIDTH, y % CHUNK_HEIGHT, z % CHUNK_WIDTH );
}

void World::destoryWorld()
{
	for( size_t i = 0; i < mWorldSize*mWorldSize*mWorldSize; i++ )
	{
		if(mRegions[i])
		{
			delete mRegions[i];
			mRegions[i] = NULL;
		}
	}
}

Magnetite::ChunkRegionArray World::getRegions() const
{
	return mRegions;
}

size_t World::getRegionCount() const
{
	return mWorldSize*mWorldSize*mWorldSize;
}

Chunk* World::getChunk(const long x, const long y, const long z)
{
	if( x < 0 || y < 0 || z < 0 ) return nullptr;
	ChunkScalar rx = x / REGION_SIZE;
	ChunkScalar ry = y / REGION_SIZE;
	ChunkScalar rz = z / REGION_SIZE;
	Magnetite::ChunkRegionPtr r = getRegion(rx, ry, rz);
	if( r == NULL ) return nullptr;
	return r->get( x % REGION_SIZE, y % REGION_SIZE, z % REGION_SIZE );
}

Magnetite::ChunkRegionPtr World::getRegion( const ChunkScalar x, const ChunkScalar y, const ChunkScalar z )
{
	size_t index = c2i( x, y, z );
	if( x < 0 || x > mWorldSize-1 || y < 0 || y > mWorldSize-1 || z < 0 || z > mWorldSize-1 )
	{
		return NULL;
	}
	else if( mRegions[index] == NULL )
	{
		// Region is valid, but has not yet been created.
		return createRegion( x, y, z );
	}

	return mRegions[index];
}

void World::requestChunk( ChunkScalar x, ChunkScalar y, ChunkScalar z )
{
	mWorldMutex.lock();
	ChunkRequest r = { x, y, z, false } ;
	mChunkRequests.push_back( r );
	mWorldMutex.unlock();
}

void World::requestChunkUnload( ChunkScalar x, ChunkScalar y, ChunkScalar z )
{
	mWorldMutex.lock();
	ChunkRequest r = { x, y, z, true } ;
	mChunkRequests.push_back( r );
	mWorldMutex.unlock();
}

Chunk* World::createChunk(long x, long y, long z)
{
	ChunkScalar rx = x / REGION_SIZE;
	ChunkScalar ry = y / REGION_SIZE;
	ChunkScalar rz = z / REGION_SIZE;
	Magnetite::ChunkRegionPtr r = getRegion(rx, ry, rz);
	if( r == NULL ) { return NULL; }
	
	// Don't create if a chunk already exists.
	auto c = r->get(x % REGION_SIZE, y % REGION_SIZE, z % REGION_SIZE);
	if( c != nullptr ) return c;
	
	// Create a new chunk and generate it (for now.);
	c = r->create( x % REGION_SIZE, y % REGION_SIZE, z % REGION_SIZE );

	return c;
}

Chunk* World::generateChunk( ChunkScalar x, ChunkScalar y, ChunkScalar z )
{
	auto c = getChunk( x, y, z );
	if( c == nullptr )
	{
		c = createChunk( x, y, z );
	}
	
	mGenerator->fillChunk( c );
	
	return c;
}

Magnetite::ChunkRegionPtr World::createRegion( const ChunkScalar x, const ChunkScalar y, const ChunkScalar z )
{
	Util::log( "Created Region: " + Util::toString(x) + " " + Util::toString(y) + " " + Util::toString(z) );
	if( x < 0 || x > mWorldSize-1 || y < 0 || y > mWorldSize-1 || z < 0 || z > mWorldSize-1 )
		return NULL;
	size_t index = c2i( x, y, z );
	return (mRegions[index] = new Magnetite::ChunkRegion( x, y, z, this ));
}

void World::removeChunk( long x, long y, long z )
{
	ChunkScalar rx = x / REGION_SIZE;
	ChunkScalar ry = y / REGION_SIZE;
	ChunkScalar rz = z / REGION_SIZE;
	Magnetite::ChunkRegionPtr r = getRegion(rx, ry, rz);
	if( r == NULL ) return;
	return r->remove( x % REGION_SIZE, y % REGION_SIZE, z % REGION_SIZE );
}

bool World::hasNeighbours(short int x, short int y, short int z)
{
	return false;
}

void World::activateChunk( long x, long y, long z )
{
	// Generate or load the chunk as it is not loaded.
	if( !mSerializer->loadChunk( x, y, z ) )
	{
		generateChunk( x, y, z );
	}
	updateAdjacent(x, y, z);
}

void World::deactivateChunk( long x, long y, long z )
{
	mSerializer->saveChunk( x, y, z );
	removeChunk( x, y, z );
}

void World::updateAdjacent( ChunkScalar x, ChunkScalar y, ChunkScalar z )
{
	Chunk* c;
	
	c = getChunk( x + 1, y, z );
	if( c ) c->_raiseChunkFlag( Chunk::DataUpdated | Chunk::SkipLight );
	c = getChunk( x - 1, y, z );
	if( c ) c->_raiseChunkFlag( Chunk::DataUpdated | Chunk::SkipLight );

	c = getChunk( x, y + 1, z );
	if( c ) c->_raiseChunkFlag( Chunk::DataUpdated | Chunk::SkipLight );
	c = getChunk( x, y - 1, z );
	if( c ) c->_raiseChunkFlag( Chunk::DataUpdated | Chunk::SkipLight );
	
	c = getChunk( x, y, z + 1 );
	if( c ) c->_raiseChunkFlag( Chunk::DataUpdated | Chunk::SkipLight );
	c = getChunk( x, y, z - 1 );
	if( c ) c->_raiseChunkFlag( Chunk::DataUpdated | Chunk::SkipLight );
}

void World::update( float dt )
{
	// Add these entries to the profiler so that they don't end up coming and going.
	Perf::Profiler::get().begin("lupdate");
	Perf::Profiler::get().end("lupdate");
	
	Perf::Profiler::get().begin("cgupdate");
	Perf::Profiler::get().end("cgupdate");
	
	Perf::Profiler::get().begin("dread");
	Perf::Profiler::get().end("dread");
	
	Perf::Profiler::get().begin("pupdate");
	Perf::Profiler::get().end("pupdate");
	
	Perf::Profiler::get().begin("vupdate");
	Perf::Profiler::get().end("vupdate");

	Perf::Profiler::get().begin("tgen");
	Perf::Profiler::get().end("tgen");
	
	// Update paging information before we do anything else.
	PagingContext::update();
	
	Perf::Profiler::get().begin("ca");
	Perf::Profiler::get().end("ca");
	
	Perf::Profiler::get().begin("cu");
	Perf::Profiler::get().end("cu");
	
	// Process the chunk loading queue
	mWorldMutex.lock();
	//Perf::Profiler::get().begin("qproc");
	if( mChunkRequests.size() > 0 )
	{
		auto &r = mChunkRequests.at(0);
		
		if( r.unload ) {
			Perf::Profiler::get().begin("cu");
			this->deactivateChunk( r.x, r.y, r.z );
			Perf::Profiler::get().end("cu");
		}
		else
		{
			Perf::Profiler::get().begin("ca");
			this->activateChunk( r.x, r.y, r.z );
			Perf::Profiler::get().end("ca");
		}
		
		mChunkRequests.pop_front();
	}
	
	//Perf::Profiler::get().end("qproc");

	mWorldMutex.unlock();
	
	Perf::Profiler::get().begin("wthink");
	auto wcube = mWorldSize*mWorldSize*mWorldSize;
	for( size_t r = 0;  r < wcube; r++ )
	{
		if( mRegions[r] == nullptr ) continue;
		for( size_t c = 0; c < mRegions[r]->count(); c++ )
		{
			auto chnk = mRegions[r]->get(c);
			if( chnk )
			{
				if( chnk->getMutex().try_lock() )
				{
					chnk->update(dt);
					chnk->getMutex().unlock();
				}
			}
		}
	}
	Perf::Profiler::get().end("wthink");
}

void World::updateEntities( float dt )
{
	if( mSky != NULL ) 
	{
		mSky->update( dt );
	}
	
	Perf::Profiler::get().begin("ethink");
	// Tick all of the entities.
	for( auto it = mEntities.begin(); it != mEntities.end(); it++ )
	{
		(*it)->think(dt);
	}
	Perf::Profiler::get().end("ethink");
}

void World::updateMovingBlocks( float dt )
{
	for( MovingBlock& b : mMovingBlocks )
	{
		float newTime = b.time - dt;
		if( newTime > 0 )
		{
		}
		else
		{
			long x = std::round( b.end.x );
			long y = std::round( b.end.y );
			long z = std::round( b.end.z );
			setBlockAt( b.block, x, y, z );
		}
	}
	for( MovingBlockList::iterator it = mMovingBlocks.begin(); it != mMovingBlocks.end(); )
	{
		auto now = ++it;
		mMovingBlocks.erase( now );
	}
}

void World::createSky( size_t time )
{
	if(mSky != NULL)
		delete mSky;
	mSky = new Sky();
	mSky->setTime( time );
}

Sky* World::getSky()
{
	return mSky;
}

Magnetite::EntityList World::getEntities()
{
	return mEntities;
}

raycast_r World::raycastWorld(const raycast_r &inray, bool solidOnly)
{
	raycast_r ray = inray;
	
	auto to = ray.orig + (ray.dir * ray.maxDistance);
	btVector3 bfrom( ray.orig.x, ray.orig.y, ray.orig.z );
	btVector3 bto( to.x, to.y, to.z );
	btCollisionWorld::ClosestRayResultCallback rayCallback(bfrom, bto);
	
	MagnetiteCore::Singleton->physicsMutex.lock();
	MagnetiteCore::Singleton->getPhysicsWorld()->rayTest( bfrom, bto, rayCallback );
	MagnetiteCore::Singleton->physicsMutex.unlock();
	
	if( rayCallback.hasHit() )
	{
		ray.hit = true;
		ray.hitNormal = glm::vec3( rayCallback.m_hitNormalWorld.x(), rayCallback.m_hitNormalWorld.y(), rayCallback.m_hitNormalWorld.z() );
		ray.worldHit = glm::vec3( rayCallback.m_hitPointWorld.x(), rayCallback.m_hitPointWorld.y(), rayCallback.m_hitPointWorld.z() );
		ray.block = nullptr;
		ray.chunk = nullptr;
		ray.i0 = rayCallback.m_closestHitFraction * ray.maxDistance;
	}
	
	return ray;
}

void World::onPageEntered( const Magnetite::PageInfo& info )
{
	this->requestChunk( info.x, info.y, info.z );
}

void World::onPageExit( const Magnetite::PageInfo& info )
{
	this->requestChunkUnload( info.x, info.y, info.z );
}