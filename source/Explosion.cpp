#include "Explosion.h"
#include "World.h"
#include "Chunk.h"
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

	float step = 0.5f;
	float length = 5.0f;
	float l = 0.0f;
	
	Vector3 explCenter = mInfo.center;
	Vector3 testPos = explCenter;
	Vector3 dir;
	
	for( float heading = 0; heading <= 2*M_PI; heading += M_PI/24.f ) {
		Matrix4 headingMat = Matrix4::rotateY(heading);
		Vector3 hdg = headingMat * Vector3(1,0,0);
		for( float pitch = -M_PI/2.f; pitch <= M_PI; pitch += M_PI/12.f ) {
			l = 0.f;
			testPos = explCenter;
			dir = Matrix4::rotateX(pitch) * hdg;
			while( l < length )
			{
				l += step;
				testPos += dir * step;
				world->removeBlockAt( testPos.x, testPos.y, testPos.z );
			}
			/*raycast_r res = world->raycastWorld(ray, true);
			if( res.hit && res.block != NULL ) {
				res.chunk->removeBlockAt( res.blockIndex );
				res.chunk->updateVisibility();
			}*/
		}
	}
}
