#include "LightingManager.h"
#include "WorldChunk.h"

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

#define point_count 64

struct IntRay {
	float left, right, top, bottom, front, back;
	IntOffset points[point_count];
};

#define ray_count 256

struct Sample {
	float left, right, top, bottom, front, back;
	IntRay rays[ray_count];
};

static int ry = 0;

void LightingManager::gatherLight( WorldChunk* chunk )
{
	BaseBlock* block = NULL;

	// Generate some useful rays.
	int n = ray_count;
	Vector3 pts[ray_count];
	float inc = 3.141f * ( 3 - sqrt( 5.f ) );
	float off = 2 / (float)n;
	for( int k = 0; k < n; k++ ) {
		float y = k * off - 1 + (off / 2);
		float r = sqrt( 1 - y*y );
		float phi = k * inc;
		pts[k].x = std::cos(phi)*r;
		pts[k].y = y;
		pts[k].z = std::sin(phi)*r;
	}

	IntRay rays[ray_count];

	size_t r = 0;

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

		rays[i] = current;
	}

	IntRay *ray, *rayend;
	IntOffset *offs, *offend;
	int xoff, yoff, zoff;
	BaseBlock* obs = NULL;
	bool collided;
	int rt = 0;
	int rc = 0;
	float right = 0, left = 0, top = 0, bottom = 0, forward = 0, back = 0;

	for( short x = 0; x < CHUNK_WIDTH; x++ ) {
		for( short y = 0; y < CHUNK_HEIGHT; y++ ) {
			for( short z = 0; z < CHUNK_WIDTH; z++ ) {
				block = chunk->getBlockAt( x, y, z );
				rt = 0;
				rc = 0;
				ry = 0;
				right = 0; left = 0; top = 0; bottom = 0; forward = 0; back = 0;
				if( block == NULL && chunk->hasNeighbours( x, y, z ) ) {
					for( ray = &rays[0], rayend = &rays[0 + ray_count]; ray < rayend; ray++ ) {
						collided = false;
						for( offs = &ray->points[0], offend = &ray->points[0 + point_count]; offs < offend; offs++ ) {
							xoff = x + offs->x;
							yoff = y + offs->y;
							zoff = z + offs->z;
							if( xoff < x - CHUNK_WIDTH/2 || xoff >= x + CHUNK_WIDTH/2 ) {
								break;
							} else 
							if( yoff < 0 || yoff >= CHUNK_HEIGHT ) {
								// Reached the outside.
								chunk->setLightLevel( x, y, z, WorldChunk::Sunlight );
								break;
							} else if( zoff < z - CHUNK_WIDTH/2 || zoff >= z + CHUNK_WIDTH/2 ) {
								break;
							}
							else {
								obs = chunk->getBlockAt( xoff, yoff, zoff );
								if( obs ) {
									collided = true;
									break;
								}
							}
						}
						if( collided == false ) {
							right += ( pts[rt].x < 0 ? -pts[rt].x : 0 );
							left += ( pts[rt].x > 0 ? pts[rt].x : 0 );
							top += ( pts[rt].y < 0 ? -pts[rt].y : 0 );
							bottom += ( pts[rt].y > 0 ? pts[rt].y : 0 );
							forward += ( pts[rt].z < 0 ? -pts[rt].z : 0 );
							back += ( pts[rt].z > 0 ? pts[rt].z : 0 );
							rc++;
						}
						rt++;
					}
					chunk->setLightLevel( x, y, z, (((right + left + top + bottom + forward + back) / ray_count)) * 256 );
					//Util::log( Util::toString( (((right + left + top + bottom + forward + back) / ray_count)) * 256 ) );
				}
			}
		}
	}
}

void LightingManager::lightChunk( WorldChunk* chunk )
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

void LightingManager::lightColumn( WorldChunk* chunk, size_t x, size_t z, LightIndex light )
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

void LightingManager::lightUnderground( WorldChunk* chunk, size_t x, size_t startY, size_t z, size_t dir, LightIndex light )
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