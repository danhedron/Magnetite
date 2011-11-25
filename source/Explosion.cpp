#include "Explosion.h"
#include "World.h"
#include "WorldChunk.h"
#include "MagnetiteCore.h"
#include "Vector.h"
#include <math.h>
#include "Renderer.h"

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
	Renderer* renderer = MagnetiteCore::Singleton->getRenderer();
	
	int totalRays = 0;
	int totalInMXZ = 0;;

	for( float heading = 0; heading <= 2*M_PI; heading += M_PI/20.f ) {
		for( float pitch = -M_PI/2.f; pitch <= M_PI; pitch += M_PI/10.f ) {
			Matrix4 headingMat = Matrix4::rotateY(heading);
			raycast_r ray;
			ray.orig = mInfo.center;
			ray.dir = headingMat * Vector3(0,0,1);
			ray.dir = Matrix4::rotateX(pitch) * ray.dir;
			raycast_r res = world->raycastWorld(ray);
			if( res.hit && res.block != NULL && res.i0 < 4.f ) {
				Vector3 chunkIndex = world->worldToChunks( res.worldHit );	
				Vector3 blockIndex = world->worldToBlock( res.worldHit - (res.hitNormal/2) );
				WorldChunk* chunk = world->getChunk( chunkIndex.x, chunkIndex.y, chunkIndex.z );
				if( chunk != NULL ) 
				{
					chunk->removeBlockAt( blockIndex.x, blockIndex.y, blockIndex.z );
				}
			}
		}
	}
}
