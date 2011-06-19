#ifndef _FRUSTUM_H_
#define _FRUSTUM_H_
#include "prerequisites.h"

class Camera;

struct Plane 
{
	enum {
		POSITIVE,
		NEGATIVE,
		BOTH
	};

	Vector3 normal;
	float d;

	float distance( const Vector3& p );
	int getSide( const Vector3& center, const Vector3& halfSize );
};

class Frustum
{
protected:
	float mFov;
	float mAspect;

	float mNear;
	float mFar;

	float mLeft;
	float mRight;
	float mTop;
	float mBottom;

	Matrix4 mPerspective;
	Plane mPlanes[6];

	Camera* mCamera;
public:

	enum {
		INSIDE,
		OUTSIDE,
		INTERSECTS
	};

	enum {
		TOP = 0,
		BOTTOM,
		LEFT,
		RIGHT,
		NEARP,
		FARP
	};

	Frustum();
	~Frustum();

	void setCamera( Camera* cam );
	void setFov(float fov);
	void setAspectRatio(float ratio);

	float getFov();
	float getAspectRatio();

	void updatePerspective();
	Matrix4 getPerspective();

	void updatePlanes();


	/**
	 * Perfoms an intersection test against the given sphere
	 */
	int intersectsSphere( const Vector3& center, float radius );
	
	/**
	 * Performs an intersection test against the given AABB
	 */
	int intersectsAABB( const Vector3& min, const Vector3& max );

	/**
	 * Performs an intersection test against the given points
	 */
	int intersectsPoints( const Vector3** points, size_t numPoints );

	/**
	 * Performs an intersection test against the given point
	 */
	int intersectsPoint( const Vector3& point );
};

#endif
