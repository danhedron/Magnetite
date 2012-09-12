#ifndef _COLLISION_H_
#define _COLLISION_H_
#include "prerequisites.h"

struct CollisionResponse
{
	Vector3 response;
	size_t mtd;
	bool collision;
};

struct AABB 
{
	Vector3 center;
	Vector3 extents;
	Vector3 min() {
		return center - (extents/2.f);
	}
	Vector3 max() {
		return center + (extents/2.f);
	}
};

class Collision
{
public:
	static void AABBvsAABB( AABB &a, AABB &b, CollisionResponse& resp );
};

#endif // _COLLISION_H_