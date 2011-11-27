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
	int totalInMXZ = 0;

	Util::log("Explosion begin");
	for( float heading = 0; heading <= 2*M_PI; heading += M_PI/12.f ) {
		Matrix4 headingMat = Matrix4::rotateY(heading);
		Vector3 hdg = headingMat * Vector3(1,0,0);
		for( float pitch = -M_PI/2.f; pitch <= M_PI; pitch += M_PI/6.f ) {
			raycast_r ray;
			ray.orig = mInfo.center;
			ray.maxDistance = 4.f;
			ray.dir = hdg;
			ray.dir = Matrix4::rotateX(pitch) * ray.dir;
			raycast_r res = world->raycastWorld(ray, true);
			if( res.hit && res.block != NULL ) {
				res.chunk->removeBlockAt( res.blockPosition.x, res.blockPosition.y, res.blockPosition.z );
				res.chunk->updateVisibility();
			}
		}
	}
	Util::log("Explosion end");
}
