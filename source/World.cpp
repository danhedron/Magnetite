#include "World.h"
#include "Chunk.h"
#include "WorldTree.h"
#include "WorldNode.h"
#include "Renderer.h"
#include "Sky.h"
#include "Camera.h"
#include "StoneBlock.h"
#include "ChunkGenerator.h"
#include "MagnetiteCore.h"
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

World::World( size_t edgeSize )
: mSky( NULL ),
mPagingCamera( NULL ),
mWorldStage( WORLD_NORMAL )
{	

	mWorldSize = edgeSize;
	mChunks = new ChunkPtr[edgeSize*edgeSize*edgeSize];
	memset( mChunks, 0, sizeof( ChunkPtr ) * edgeSize*edgeSize*edgeSize); 
	
// 	for( size_t x = 0; x < edgeSize; x++ ) {
// 		for( size_t z = 0; z < edgeSize; z++ ) {
// 			for( size_t y = 0; y < edgeSize; y++ ) {
// 				size_t idx = coordsToIndex( x, y, z );
// 				mChunks[idx] = NULL;
// 			}
// 		}
// 	}
	
	createSky( 0 );

	mGenerator = new ChunkGenerator(1024);
	mGenerator->fillRegion( this, Vector3(0,0,0), Vector3( (edgeSize-1) * CHUNK_WIDTH, (edgeSize-1) * CHUNK_WIDTH, (edgeSize-1) * CHUNK_WIDTH ) );
	
	printDbg = false;
}

World::~World()
{
	destoryWorld();	
}

size_t World::coordsToIndex( int x, int y, int z )
{
	return ( z * mWorldSize * mWorldSize + y * mWorldSize + x );
}

WorldStage World::getCurrentStage()
{
	return mWorldStage;
}

BlockPtr World::getBlockAt( long x, long y, long z )
{
	size_t cx = floor( ((float) x)/CHUNK_WIDTH );
	size_t cy = floor( ((float) y)/CHUNK_HEIGHT );
	size_t cz = floor( ((float) z)/CHUNK_WIDTH );
	Chunk* c = getChunk( cx, cy, cz );
	if( c == NULL ) return NULL;
	else return c->getBlockAt( x % CHUNK_WIDTH, y % CHUNK_HEIGHT, z % CHUNK_WIDTH );
}

void World::removeBlockAt( long x, long y, long z )
{
	size_t cx = floor( ((float) x)/CHUNK_WIDTH );
	size_t cy = floor( ((float) y)/CHUNK_HEIGHT );
	size_t cz = floor( ((float) z)/CHUNK_WIDTH );
	Chunk* c = getChunk( cx, cy, cz );
	if( c == NULL ) return;
	else c->removeBlockAt( x % CHUNK_WIDTH, y % CHUNK_HEIGHT, z % CHUNK_WIDTH );
}

void World::setBlockAt( BaseBlock* b, long x, long y, long z )
{
	size_t cx = floor( ((float) x)/CHUNK_WIDTH );
	size_t cy = floor( ((float) y)/CHUNK_HEIGHT );
	size_t cz = floor( ((float) z)/CHUNK_WIDTH );
	Chunk* c = getChunk( cx, cy, cz );
	if( c == NULL ) return;
	else c->setBlockAt( b, x % CHUNK_WIDTH, y % CHUNK_HEIGHT, z % CHUNK_WIDTH );
}

LightIndex World::getLightLevel( long x, long y, long z )
{
	size_t cx = floor( ((float) x)/CHUNK_WIDTH );
	size_t cy = floor( ((float) y)/CHUNK_HEIGHT );
	size_t cz = floor( ((float) z)/CHUNK_WIDTH );
	Chunk* c = getChunk( cx, cy, cz );
	if( c == NULL ) return 0;
	return c->getLightLevel( x % CHUNK_WIDTH, y % CHUNK_HEIGHT, z % CHUNK_WIDTH );
}

float World::getLightColor( LightIndex light )
{
	return ( 0.1f + ( 0.9f * ( (float)light/256 ) ) );
}

void World::destoryWorld()
{
	for( size_t i = 0; i < mWorldSize*mWorldSize*mWorldSize; i++ )
	{
		if(mChunks[i])
		{
			delete mChunks[i];
			mChunks[i] = NULL;
		}
	}
}

ChunkArray World::getChunks()
{
	return mChunks;
}

size_t World::getChunkCount()
{
	return mWorldSize*mWorldSize*mWorldSize;
}

Chunk* World::getChunk(const long x, const long y, const long z)
{
	size_t index = coordsToIndex( x, y, z );
	if( x < 0 || x > mWorldSize-1 || y < 0 || y > mWorldSize-1 || z < 0 || z > mWorldSize-1 )
	{
		return NULL;
	}
	else if( mChunks[index] == NULL )
	{
		// Chunk is inside the world bounds, but hasn't been created yet.
		return createChunk( x, y, z );
	}

	return mChunks[index];
}

Chunk* World::createChunk(long x, long y, long z)
{
	size_t index = coordsToIndex( x, y, z );
	if( x < 0 || x > mWorldSize-1 || y < 0 || y > mWorldSize-1 || z < 0 || z > mWorldSize-1 )
		return NULL;
	mChunks[index] = new Chunk( ChunkIndex{ x, y, z } ); // 6859 (thanks to n3hima)
	
	return mChunks[index];
}

void World::removeChunk( long x, long y, long z )
{
	size_t index = coordsToIndex( x, y, z );
	if( index < 0 || index > mWorldSize*mWorldSize*mWorldSize )
		return;
	delete mChunks[index];
	mChunks[index] = NULL;
}

void World::setPagingCamera( Camera* _c )
{
	mPagingCamera = _c;
}

void World::activateChunk( long x, long y, long z )
{
	// Generate or load the chunk as it is not loaded.
	createChunk( x, y, z );
}

void World::deativateChunk( long x, long y, long z )
{
	removeChunk( x, y, z );
}

void World::createWorldFolder()
{
	char currPath[FILENAME_MAX];
	getcwd(currPath, sizeof currPath);
	std::string path = currPath;
//	if( mkdir( (path + "\\worlds\\").c_str() ) != ENOENT ) {
//		if( mkdir( (path + "\\worlds\\" + mWorldName + "\\").c_str() ) != ENOENT ) {
//			mkdir( (path + "\\worlds\\" + mWorldName + "\\chunks").c_str() );
//		}
//	}
}

void World::update( float dt )
{
	if( mSky != NULL )
		mSky->update( dt );

	// Proccess the chunk loading queue
	if( mChunksToLoad.size() > 0 ) {
		mWorldStage = WORLD_GEN;
		for( int i = 0; mChunksToLoad.size() > 0 && i < 10; i++ ) {
			ChunkRequest req = mChunksToLoad.back();
			Chunk* c = createChunk( req.x, req.y, req.z);
			c->requestGenerate();
			mChunksToLoad.pop_back();
		}
	
		//for( ChunkList::iterator it = mChunks.begin(); it != mChunks.end(); ++it ) {
		//	(*it)->requestGenerate();
		//}
		mWorldStage = WORLD_NORMAL;
	}
	else
	{
		auto wcube = mWorldSize*mWorldSize*mWorldSize;

		for( int i = 0; i < wcube; i++ )
		{
			if( mChunks[i] )
			{
				mChunks[i]->update(dt);
			}
		}
		for( int i = 0; i < wcube; i++ )
		{
			if( mChunks[i] )
			{
				mChunks[i]->requestGenerate();
			}
		}
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

raycast_r World::raycastWorld(const raycast_r &inray, bool solidOnly)
{
	raycast_r ray = inray;
	Vector3 min, max;
	auto wcube = mWorldSize*mWorldSize*mWorldSize;
	std::vector<Chunk*> hitChunks;
	
	for( int i = 0; i < wcube; i++ )
	{
		if( !mChunks[i] ) { continue; }
		Chunk* c = mChunks[i];
		min = Vector3( c->getX() * CHUNK_WIDTH, c->getY() * CHUNK_HEIGHT, c->getZ() * CHUNK_WIDTH );
		max = Vector3( c->getX() * CHUNK_WIDTH + CHUNK_WIDTH, c->getY() * CHUNK_HEIGHT + CHUNK_WIDTH, c->getZ() * CHUNK_WIDTH + CHUNK_WIDTH );
		ray = raycastCube( ray, min, max );
		if( ray.hit && ray.i0 <= inray.maxDistance )
		{
			hitChunks.push_back( c );
		}
	}
	
	ray.hit = false;
	raycast_r closest;
	float dist = std::numeric_limits<float>::max();
	
	for( int i = 0; i < hitChunks.size(); i++ )
	{
		Chunk* hitChunk = hitChunks[i];
		BlockArray blocks = hitChunk->getBlocks();
		
		for( size_t c = 0; c < CHUNK_SIZE-1; c++ )
		{
			if( (blocks)[c] == NULL ) continue;
			BaseBlock* b = (blocks)[c];

			if( solidOnly && !b->isSolid() )
				continue;

			Vector3 bPos = Util::indexToPosition( c );
			min = Vector3( hitChunk->getX() * CHUNK_WIDTH - 0.0f,
					hitChunk->getY() * CHUNK_HEIGHT + 0.0f,
					hitChunk->getZ() * CHUNK_WIDTH - 0.0f ) + bPos;
			max = Vector3( hitChunk->getX() * CHUNK_WIDTH + 1.0f,
					hitChunk->getY() * CHUNK_HEIGHT + 1.0f,
					hitChunk->getZ() * CHUNK_WIDTH + 1.0f ) + bPos;
			raycast_r r = inray;
			r = raycastCube(r, min, max);
			if( r.hit == true && r.i0 < dist ) {
				r.block = b;
				r.blockPosition = bPos;
				r.blockIndex = c;
				r.chunk = hitChunk;
				dist = r.i0;
				closest = r;
			}
		}
	}
	
	return closest;
}

CollisionResponse World::AABBWorld( Vector3& min, Vector3& max )
{
	/*ChunkList hitChunks;
	AABB targetBB;
	targetBB.center = (min + (max-min)/2);
	targetBB.extents = (max-min);
	CollisionResponse r;
	for(ChunkList::iterator it = mChunks.begin(); it != mChunks.end(); ++it)
	{
		AABB chunkAABB;
		chunkAABB.center = Vector3( (*it)->getX() * CHUNK_WIDTH + CHUNK_WIDTH/2, (*it)->getY() * CHUNK_HEIGHT + CHUNK_HEIGHT/2, (*it)->getZ() * CHUNK_WIDTH + CHUNK_WIDTH/2 );
		chunkAABB.extents = Vector3( CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_WIDTH );
		Collision::AABBvsAABB( chunkAABB, targetBB, r );
		if( r.collision ) {
			hitChunks.push_back( *it );
		}
	}
	r.response = Vector3();

	BlockList* blocks = NULL;
	AABB bb;
	CollisionResponse final;
	final.collision = false;
	Vector3 ofs( 0,0,0 );

	for(ChunkList::iterator it = hitChunks.begin(); it != hitChunks.end(); ++it)
	{
		blocks = (*it)->getVisibleBlocks();
		Vector3 chunkPos( (*it)->getX() * CHUNK_WIDTH, (*it)->getY() * CHUNK_HEIGHT, (*it)->getZ() * CHUNK_WIDTH );
		for(BlockList::iterator block = blocks->begin(); block != blocks->end(); ++block) {
			bb.center = chunkPos + Util::indexToPosition( block->first )
				+ Vector3( 0.5f, 0.5f, 0.5f );
			bb.extents = Vector3( 1.0f, 1.0f, 1.0f );
			Collision::AABBvsAABB( bb, targetBB, r );
			if( r.collision ) {
				final.collision = true;
				ofs.x = std::max( ofs.x, r.response.x );
				ofs.y = std::max( ofs.y, r.response.y );
				ofs.z = std::max( ofs.z, r.response.z );
			}
		}	
	}

	if( ( ofs.x > ofs.y ) && ( ofs.x > ofs.z ) ) {
		final.response.x = ofs.x;
	}
	else if( ( ofs.y > ofs.x ) && ( ofs.y > ofs.z ) ) {
		final.response.y = ofs.y;
	}
	else if( ( ofs.z > ofs.x ) && ( ofs.z > ofs.y ) ) {
		final.response.z = ofs.z;
	}


	return final;*/
	return CollisionResponse();
}

Vector3 normals[3] = {
	Vector3(1.f, 0.f, 0.f), // X
	Vector3(0.f, 1.f, 0.f), // Y
	Vector3(0.f, 0.f, 1.f) // Z
};

raycast_r World::raycastCube(const raycast_r &inray, Vector3& min, Vector3& max)
{
	raycast_r ray(inray);

	float Tnear = INT_MIN; // These values are plenty large enough.
	float Tfar	= INT_MAX;

	for(int p = 0; p < 3; p++) {
		Vector3 norm = normals[p]; 
		if( ray.dir[p] == 0 ) {
			// Ray is parallel to this plane
			if( ray.orig[p] < min[p] || ray.orig[p] > max[p] )
			{
				// Ray is outside of the planes of this axis, it can't intersect.
				ray.hit = false; 
				return ray;
			}
		}
		else
		{
			float t1 = (min[p] - ray.orig[p]) / ray.dir[p];
			float t2 = (max[p] - ray.orig[p]) / ray.dir[p];
			if( t1 > t2 ) {
				std::swap( t1, t2 );
			}
			else { norm = -norm; }
			if( t1 > Tnear ) {
				Tnear = t1;
				ray.hitNormal = norm;
			}
			if( t2 < Tfar ) Tfar = t2;
			if( Tnear > Tfar || Tfar < 0 ) {
				ray.hit = false;
				return ray;
			}
		}
	}

	ray.hitNormal = ray.hitNormal.normalize();

	ray.hit = true;
	ray.i0 = Tnear;
	ray.i1 = Tfar;
	ray.worldHit = ray.orig + ( ray.dir * ray.i0 );
	return ray;
}

Vector3 World::worldToChunks(const Vector3 &vec)
{
	Vector3 v;
	v.x = floor( vec.x / (float)CHUNK_WIDTH );
	v.y = floor( vec.y / (float)CHUNK_HEIGHT );
	v.z = floor( vec.z / (float)CHUNK_WIDTH );
	return v;
}

Vector3 World::worldToBlock(const Vector3 &vec)
{
	Vector3 v;
	v.x = abs((int)vec.x % CHUNK_WIDTH);// * ( vec.x < 0 ? -1.f : 1.f );
	v.x = vec.x < 0 ? CHUNK_WIDTH - v.x - 1: v.x;
	v.y = abs((int)vec.y % CHUNK_HEIGHT);// * ( vec.y < 0 ? -1.f : 1.f );
	v.y = vec.y < 0 ? CHUNK_HEIGHT - v.y - 1 : v.y;
	v.z = abs((int)vec.z % CHUNK_WIDTH);// * ( vec.z < 0 ? -1.f : 1.f );
	v.z = vec.z < 0 ? CHUNK_WIDTH - v.z - 1: v.z;
	return v;
}
