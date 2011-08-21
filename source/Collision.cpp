#include "Collision.h"
#include "Vector.h"

bool overlap(Vector3& mina, Vector3& maxa, Vector3& minb, Vector3& maxb)
{
	Vector3 dir = (minb)-mina;
	return abs(dir.x) <= ((maxa.x-mina.x) + (maxb.x-minb.x)) 
		&& abs(dir.y) <= ((maxa.y-mina.y) + (maxb.y-minb.y)) 
		&& abs(dir.z) <= ((maxa.z-mina.z) + (maxb.z-minb.z));
}

void Collision::AABBvsAABB( AABB &a, AABB &b, CollisionResponse& resp )
{
	resp.collision = overlap( a.min(), a.max(), b.min(), b.max() );

	if( resp.collision ) {
		Vector3 d = a.center - b.center;
		resp.response = Vector3();

		Vector3 mina = a.min();
		Vector3 minb = b.min();
		Vector3 maxa = a.max();
		Vector3 maxb = b.max();


		Vector3 e2 = Vector3( std::min( maxa.x, maxb.x ), std::min( maxa.y, maxb.y ), std::min( maxa.z, maxb.z ) );
		Vector3 e1 = Vector3( std::max( mina.x, minb.x ), std::max( mina.y, minb.y ), std::max( mina.z, minb.z ) );

		Vector3 ovl = e2-e1;

		if( ( ovl.x < ovl.y ) && ( ovl.x < ovl.z ) ) {
			resp.mtd = 0;
			resp.response.x = ovl.x; //(( ovl.x < 0.f ? -1.f : 1.f ));// * ( (a.extents.x + b.extents.x)/2 ));
		}
		if( ( ovl.y < ovl.x ) && ( ovl.y < ovl.z ) ) {
			resp.mtd = 1;
			resp.response.y = ovl.y; // (( ovl.y < 0.f ? -1.f : 1.f ));// * ( (a.extents.y + b.extents.y)/2 ));
		}
		if( ( ovl.z < ovl.y ) && ( ovl.x < ovl.z ) ) {
			resp.mtd = 2;
			resp.response.z = ovl.z; // (( ovl.z < 0.f ? -1.f : 1.f ));// * ( (a.extents.z + b.extents.z)/2 ));
		}
	}
}