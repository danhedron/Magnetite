#include "World.h"
#include "WorldChunk.h"
#include "Renderer.h"
#include "Sky.h"
#include "StoneBlock.h"

World::World()
: mSky( NULL )
{	
	createWorld();
}

World::~World()
{

}

void World::createWorld()
{
	createSky(200);
	//Create some testing chunks
	createTestChunks( 2 );
}

void World::createTestChunks( int size )
{
	destoryWorld();
	for(int i = -size; i < size; i++) {
		for(int z = -size; z < size; z++) {
			Util::log("New Chunk");
			createChunk(i,0,z);
		}
	}
}

void World::destoryWorld()
{
	for(ChunkList::iterator it = mChunks.begin(); it != mChunks.end(); )
	{
		Util::log("Deleted Chunk");
		delete (*it);
		it = mChunks.erase( it );
	}
}

ChunkList& World::getChunks()
{
	return mChunks;
}

WorldChunk* World::getChunk(const long x, const long y, const long z)
{
	for(ChunkList::iterator it = mChunks.begin(); it != mChunks.end(); ++it)
	{
		if( x == (*it)->getX() && y == (*it)->getY() && z == (*it)->getZ() ) {
			return (*it);
		}
	}
	return NULL;
}

void World::createChunk(long x, long y, long z)
{
	Util::log( "Creating Chunk: " + Util::toString(x) + "," + Util::toString(y) + "," + Util::toString(z) );
	WorldChunk* newChunk = new WorldChunk(x, y, z);
	Util::log("Adding test data..");
	newChunk->fillWithTestData();
	mChunks.push_back(newChunk);
	Util::log( "Chunk Created" );
}

void World::update( float dt )
{
	if( mSky != NULL )
		mSky->update( dt );
	for( ChunkList::iterator it = mChunks.begin(); it != mChunks.end(); ++it ) {
		(*it)->update(dt);
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

void World::removeChunk(long x, long y, long z)
{
	for(ChunkList::iterator it = mChunks.begin(); it != mChunks.end(); ++it)
	{
		if( x == (*it)->getX() && y == (*it)->getY() && z == (*it)->getZ() ) {
			Util::log("Deleted Chunk");
			delete (*it);
			mChunks.erase( it );
			return;
		}
	}
}

raycast_r World::raycastWorld(const raycast_r &inray)
{
	raycast_r ray = inray;
	Vector3 min, max;
	ChunkList hitChunks;
	for(ChunkList::iterator it = mChunks.begin(); it != mChunks.end(); ++it)
	{
		min = Vector3( (*it)->getX() * CHUNK_WIDTH, (*it)->getY() * CHUNK_HEIGHT,  (*it)->getZ() * CHUNK_WIDTH );
		max = Vector3( (*it)->getX() * CHUNK_WIDTH + CHUNK_WIDTH, (*it)->getY() * CHUNK_HEIGHT + CHUNK_HEIGHT,  (*it)->getZ() * CHUNK_WIDTH + CHUNK_WIDTH );
		ray = raycastCube(ray, min, max);
		if(ray.hit)
			hitChunks.push_back((*it));
	}
	BlockList* blocks = NULL;
	ray.hit = false;
	std::vector<raycast_r> raycasts;
	for(ChunkList::iterator it = hitChunks.begin(); it != hitChunks.end(); ++it)
	{
		blocks = (*it)->getVisibleBlocks();
		for(BlockList::iterator block = blocks->begin(); block != blocks->end(); ++block) {
			min = Vector3( (*it)->getX() * CHUNK_WIDTH + (*block).second->getX() - 0.0f,
							(*it)->getY() * CHUNK_HEIGHT + (*block).second->getY() - 0.0f,
							(*it)->getZ() * CHUNK_WIDTH + (*block).second->getZ() - 0.0f );
			max = Vector3( (*it)->getX() * CHUNK_WIDTH + (*block).second->getX() + 1.0f,
							(*it)->getY() * CHUNK_HEIGHT + (*block).second->getY() + 1.0f,
							(*it)->getZ() * CHUNK_WIDTH + (*block).second->getZ() + 1.0f );
			raycast_r r = inray;
			r = raycastCube(r, min, max);
			if( r.hit == true )
				raycasts.push_back( r );
		}	
	}
	// Final pass, find closest hit.
	float m = std::numeric_limits<float>::max();
	raycast_r closest;
	for(std::vector<raycast_r>::iterator it = raycasts.begin(); it != raycasts.end(); ++it)
	{
		if((*it).i0 < m) {
			m = it->i0;
			closest = (*it);
		}
	}

	return closest;
}

raycast_r World::raycastCube(const raycast_r &inray, Vector3& min, Vector3& max)
{
	raycast_r ray(inray);
	const Vector3 l1((min - ray.orig) / ray.dir);
	const Vector3 l2((max - ray.orig) / ray.dir);

	Vector3 v_far;
	v_far.x = std::max( l1.x, l2.x );
	v_far.y = std::max( l1.y, l2.y );
	v_far.z = std::max( l1.z, l2.z );
	Vector3 v_near;
	v_near.x = std::min( l1.x, l2.x );
	v_near.y = std::min( l1.y, l2.y );
	v_near.z = std::min( l1.z, l2.z );
	ray.i1 = std::min( v_far.smallestDimension(), ray.maxDistance );
	ray.i0 = std::max( v_near.biggestDimension(), 0.f );
	ray.hit = (ray.i1 >= ray.i0) & (ray.i1 >= 0.f );
	ray.worldHit = ray.orig + ( ray.dir * ray.i0 );
	return ray;
}

Vector3 World::worldToChunks(const Vector3 &vec)
{
	Vector3 v;
	v.x = (vec.x / CHUNK_WIDTH) - ( vec.x < 0 ? 1 : 0 );
	v.y = (vec.y / CHUNK_HEIGHT) - ( vec.y < 0 ? 1 : 0 );
	v.z = (vec.z / CHUNK_WIDTH) - ( vec.z < 0 ? 1 : 0 );
	return v;
}

Vector3 World::worldToBlock(const Vector3 &vec)
{
	Vector3 v;
	v.x = ( vec.x < 0 ? CHUNK_WIDTH-1 : 0 ) + ( vec.x < 0 ? -1 : 1 ) * abs((int)vec.x)%CHUNK_WIDTH;
	v.y = ( vec.y < 0 ? CHUNK_HEIGHT-1 : 0 ) + ( vec.y < 0 ? -1 : 1 ) * abs((int)vec.y)%CHUNK_HEIGHT;
	v.z = ( vec.z < 0 ? CHUNK_WIDTH-1 : 0 ) + ( vec.z < 0 ? -1 : 1 ) * abs((int)vec.z)%CHUNK_WIDTH;
	return v;
}