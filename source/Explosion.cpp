#include "Explosion.h"
#include "World.h"
#include "Chunk.h"
#include "MagnetiteCore.h"
#include "Vector.h"
#include <math.h>
#include <glm/gtc/matrix_transform.hpp>
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
		Matrix4 headingMat = glm::rotate( glm::mat4(), heading, glm::vec3( 0.f, 1.f, 0.f ) );
		Vector3 hdg = glm::mat3(headingMat) * Vector3(1,0,0);
		for( float pitch = -M_PI/2.f; pitch <= M_PI; pitch += M_PI/12.f ) {
			l = 0.f;
			testPos = explCenter;
			dir = glm::mat3( glm::rotate( glm::mat4(), pitch, glm::vec3( 1.f, 0.f, 0.f ) ) ) * hdg;
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
