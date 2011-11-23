#include "Explosion.h"
#include "World.h"
#include "WorldChunk.h"
#include "MagnetiteCore.h"
#include "Vector.h"
#include <math.h>

Explosion::Explosion( explosion_t info )
{
	mInfo = info;
}

Explosion::~Explosion()
{
}

void Explosion::explode()
{
	World* world = MagnetiteCore::Singleton->getWorld();
	
	int totalRays = 0;
	int totalInMXZ = 0;;

	for( float y = 0; y <= 2*M_PI; y += M_PI/12.f ) {
		Matrix4 heading = Matrix4::rotateY(y); 
		for( float x = -M_PI; x <= M_PI; x += M_PI/6.f ) {
			//Matrix4 mat = heading; //* Matrix4::rotateX(x);
			raycast_r ray;
			ray.dir = Vector3(0,0,1);
			ray.dir = Matrix4::rotateX(x) * ray.dir;
			ray.dir = heading * ray.dir;
			ray.orig = mInfo.center;
			raycast_r res = world->raycastWorld(ray);
			if( res.hit && res.block != NULL && res.i0 < 5.f ) {
				Util::log("dir: " + Util::toString( ray.dir ) + "\tworld: " + Util::toString(res.worldHit) + "\tlength: " + Util::toString(res.i0));
				Vector3 chunkIndex = world->worldToChunks( res.worldHit );	
				Vector3 blockIndex = world->worldToBlock( res.worldHit );
				WorldChunk* chunk = world->getChunk( chunkIndex.x, chunkIndex.y, chunkIndex.z );
				if( chunk != NULL ) 
				{
					chunk->removeBlockAt( blockIndex.x, blockIndex.y, blockIndex.z );
				}
			}
		}
	}

}
