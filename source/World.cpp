#include "World.h"
#include "WorldChunk.h"
#include "Renderer.h"
#include "StoneBlock.h"

World::World()
{	
	createWorld();
}

World::~World()
{

}

void World::createWorld()
{
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

raycast_r World::raycastWorld(raycast_r &ray)
{
	Vector3 min, max;
	ChunkList hitChunks;
	for(ChunkList::iterator it = mChunks.begin(); it != mChunks.end(); ++it)
	{
		min = Vector3( (*it)->getX() * CHUNK_WIDTH, (*it)->getY() * CHUNK_HEIGHT,  (*it)->getZ() * CHUNK_WIDTH );
		max = Vector3( (*it)->getX() * CHUNK_WIDTH + CHUNK_WIDTH, (*it)->getY() * CHUNK_HEIGHT + CHUNK_HEIGHT,  (*it)->getZ() * CHUNK_WIDTH + CHUNK_WIDTH );
		raycastCube(ray, min, max);
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
			min = Vector3( (*it)->getX() * CHUNK_WIDTH + (*block).second->getX() - 0.5f,
							(*it)->getY() * CHUNK_HEIGHT + (*block).second->getY() - 0.5f,
							(*it)->getZ() * CHUNK_WIDTH + (*block).second->getZ() - 0.5f );
			max = Vector3( (*it)->getX() * CHUNK_WIDTH + (*block).second->getX() + 0.5f,
							(*it)->getY() * CHUNK_HEIGHT + (*block).second->getY() + 0.5f,
							(*it)->getZ() * CHUNK_WIDTH + (*block).second->getZ() + 0.5f );
			raycast_r r = ray;
			raycastCube(r, min, max);
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
			closest = (*it);
		}
	}

	return closest;
}

raycast_r& World::raycastCube(raycast_r &ray, Vector3& min, Vector3& max)
{
	float t0 = 0;
	float t1 = ray.maxDistance;
	for(int i = 0; i < 3; ++i)
	{
		float invRayDir = 1.f / ray.dir[i];
		float near_t = (min[i] - ray.orig[i]) * invRayDir;
		float far_t = (max[i] - ray.orig[i]) * invRayDir;
		if(near_t > far_t)				
		{					
			float temp = near_t;
			near_t = far_t;
			far_t = temp;
		}
		t0 = near_t > t0 ? near_t : t0;
		t1 = far_t < t1 ? far_t : t1;
		if(t0 > t1) 
		{
			ray.hit = false;
			return ray;
		}
	}			
	ray.i0 = t0;
	ray.i1 = t1;
	ray.hit = true;
	ray.worldHit = ray.orig + ray.dir*ray.i0;
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