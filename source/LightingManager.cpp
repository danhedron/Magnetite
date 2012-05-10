#include "LightingManager.h"
#include "MagnetiteCore.h"
#include "BaseBlock.h"
#include "World.h"
#include "Chunk.h"
#include "math.h"
#include "Profiler.h"

LightingManager::LightingManager()
: litChunks(0)
{
}

LightingManager::~LightingManager()
{
}

struct IntOffset {
		float depth;
		int x, y, z;
};

#define point_count 16

struct IntRay {
	float left, right, top, bottom, front, back;
	IntOffset points[point_count];
};

#define ray_count 16

struct Sample {
	float left, right, top, bottom, front, back;
	Sample() {
		right = 0; left = 0; top = 0; bottom = 0; front = 0; back = 0;
	}
	float getSample( float right, float left, float top, float bottom, float front, float back) {
		float t = 0;
		t += ( right / this->right );
		t += ( left / this->left );
		t += ( top / this->top );
		t += ( bottom / this->bottom );
		t += ( front / this->front );
		t += ( back / this->back );
		t /= 6;
		return t;
	}
};

void LightingManager::gatherLight( Chunk* chunk )
{
	BaseBlock* block = NULL;
	static Sample smp;
	static IntRay rays[ray_count];
	static bool madeRays = false;
	World* world = MagnetiteCore::Singleton->getWorld();

	if( !madeRays ) {
		// Generate some useful rays.
		int n = ray_count;
		Vector3 pts[ray_count];
		float inc = 3.141f * ( 3 - sqrt( 5.f ) );
		float off = 2 / (float)n;
		for( int k = 0; k < n; k++ ) {
			float y = k * off - 1 + (off / 2);
			float r = sqrt( 1 - y*y );
			float phi = k * inc;
			pts[k].x = cos(phi)*r;
			pts[k].y = y;
			pts[k].z = sin(phi)*r;
		}

		for( int i = 0; i < ray_count; i++ ) {
			IntRay current;

			float sx = pts[i].x * 0.2f;
			float sy = pts[i].y * 0.2f;
			float sz = pts[i].z * 0.2f;

			float x = 0, y = 0, z = 0;

			int cx = 0, cy = 0, cz = 0;

			int p = 0;
			while( p < point_count ) {
				int nx = x;
				int ny = y;
				int nz = z;
				if( nx != cx || ny != cy || nz != cz ) {
					current.points[p].depth = sqrt( x*x + y*y + z*z );
					current.points[p].x = nx;
					current.points[p].y = ny;
					current.points[p].z = nz;
					cx = nx;
					cy = ny;
					cz = nz;
					p++;
				}
				x += sx;
				y += sy;
				z += sz;
			}

			current.right	= ( pts[i].x < 0 ? -pts[i].x : 0 );
			current.left	= ( pts[i].x > 0 ? pts[i].x : 0 );
			current.top		= ( pts[i].y < 0 ? -pts[i].y : 0 );
			current.bottom	= ( pts[i].y > 0 ? pts[i].y : 0 );
			current.front	= ( pts[i].z < 0 ? -pts[i].z : 0 );
			current.back	= ( pts[i].z > 0 ? pts[i].z : 0 );

			smp.right += current.right;
			smp.left += current.left;
			smp.top += current.top;
			smp.bottom += current.bottom;
			smp.front += current.front;
			smp.back += current.back;

			rays[i] = current;
		}
		madeRays = true;
	}

	IntRay *ray, *rayend;
	IntOffset *offs, *offend;
	BaseBlock* obs = NULL;
	long pX = (chunk->getX() * CHUNK_WIDTH);
	long pY = (chunk->getY() * CHUNK_HEIGHT);
	long pZ = (chunk->getZ() * CHUNK_WIDTH);
	Perf::Profiler::get().begin("Chunk Lighting");
	if( chunk->getBlockCount() > 0 && chunk->getBlockCount() < CHUNK_SIZE )
	{
		float right = 0, left = 0, top = 0, bottom = 0, front = 0, back = 0;
		for( short x = 0; x < CHUNK_WIDTH; x++ ) {
			for( short y = 0; y < CHUNK_HEIGHT; y++ ) {
				for( short z = 0; z < CHUNK_WIDTH; z++ ) {
					block = chunk->getBlockAt( x, y, z );
					if( block == NULL && chunk->hasNeighbours( x, y, z ) ) {
						long wx = x + pX;
						long wy = y + pY;
						long wz = z + pZ;
						right = 0; left = 0; top = 0; bottom = 0; front = 0; back = 0;
						for( ray = &rays[0], rayend = &rays[0 + ray_count]; ray < rayend; ray++ ) {
							for( offs = &ray->points[0], offend = &ray->points[0 + point_count]; offs < offend; offs++ ) {
								obs = world->getBlockAt( wx + offs->x, wy + offs->y, wz + offs->z );
								if( obs ) break;
							}
							if( obs == nullptr )
							{
								right += ray->right;
								left += ray->left;
								top += ray->top;
								bottom += ray->bottom;
								front += ray->front;
								back += ray->back;
							}
						}
						LightIndex lighness = smp.getSample(right, left, top, bottom, front, back) * 255;
						chunk->setLightLevel(lighness, x, y, z);
					}
				}
			}
		}
	}
	Perf::Profiler::get().end("Chunk Lighting");
}

void LightingManager::lightChunk( Chunk* chunk )
{
	/*for( size_t x = 0; x < CHUNK_WIDTH; x++ ) 
	{
		for( size_t z = 0; z < CHUNK_WIDTH; z++ ) 
		{
			lightColumn( chunk, x,  z, WorldChunk::Sunlight );
		}
	}*/
	gatherLight( chunk );
}
/*
void LightingManager::lightColumn( Chunk* chunk, size_t x, size_t z, LightIndex light )
{
	LightIndex currentLight = WorldChunk::Sunlight;
	char directionFlags = 0; // Stores directions in which take us under cover.
	bool oh_my_its_dark_in_here = false; // Have we gone under-cover
	for( short y = CHUNK_HEIGHT - 1; y >= 0; y-- )
	{
		if( chunk->getBlockAt(x, y, z) ) 
		{
			chunk->setLightLevel( x, y, z, 0 );
			oh_my_its_dark_in_here = true;
		}
		else if( !oh_my_its_dark_in_here )
		{
			// Sunlight
			chunk->setLightLevel( x, y, z, WorldChunk::Sunlight );
		}
		else
		{
			chunk->setLightLevel( x, y, z, 0 );
		}

		if( !oh_my_its_dark_in_here ) {
			//Check -X 
			if( chunk->getBlockAt(x - 1, y, z) )
			{
				directionFlags |= NEG_X;
			}
			else if( (directionFlags & NEG_X) == NEG_X )
			{
				// light shadow
				lightUnderground( chunk, x - 1, y, z, NEG_X, WorldChunk::Sunlight | 1 );
			}

			//Check +X 
			if( chunk->getBlockAt(x+1, y, z) )
			{
				directionFlags |= POS_X;
			}
			else if( (directionFlags & POS_X) == POS_X )
			{
				// light shadow
				lightUnderground( chunk, x + 1, y, z, POS_X, WorldChunk::Sunlight | 1 );
			}

			//Check -Z
			if( chunk->getBlockAt(x, y, z-1) )
			{
				directionFlags |= NEG_Z;
			}
			else if( (directionFlags & NEG_Z) == NEG_Z )
			{
				// light shadow
				lightUnderground( chunk, x, y, z - 1, NEG_Z, WorldChunk::Sunlight | 1 );
			}

			//Check +Z 
			if( chunk->getBlockAt(x, y, z+1) )
			{
				directionFlags |= POS_Z;
			}
			else if( (directionFlags & POS_Z) == POS_Z )
			{
				// light shadow
				lightUnderground( chunk, x, y, z + 1, POS_Z, WorldChunk::Sunlight | 1 );
			}
		}
	}
}

void LightingManager::lightUnderground( Chunk* chunk, size_t x, size_t startY, size_t z, size_t dir, LightIndex light )
{
	LightIndex currentLight = light;
	for( short y = startY; y >= 0; y-- )
	{
		// if we hit a block, stop
		if( chunk->getBlockAt( x, y, z ) )
			return;
		// Set the light level 
		chunk->setLightLevel( x, y, z, currentLight );
	}
}
*/