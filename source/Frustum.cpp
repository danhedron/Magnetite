#include "Frustum.h"
#include "Camera.h"

float Plane::distance( const Vector3& p )
{
	return (normal.dotProduct( p ) + d);
}

int Plane::getSide( const Vector3& center, const Vector3& halfSize )
{
	float dist = distance(center);

	float maxDist = abs(normal.dotProduct(halfSize));
	if( dist < -maxDist ) 
		return NEGATIVE;
	
	if( dist > maxDist )
		return POSITIVE;

	return BOTH;
}

Frustum::Frustum()
: mAspect( 1.f ),
mNear( 0.01f ),
mFar( 1000.f ),
mTop( 0 ),
mBottom( 0 ),
mLeft( 0 ),
mRight( 0 ),
mCamera( NULL )
{
	setFov( 90.f );
}

Frustum::~Frustum()
{

}

void Frustum::setFov(float fov)
{
	mFov = (3.141f/180.f) * fov;
}

void Frustum::setAspectRatio(float ratio)
{
	mAspect = ratio;
}

void Frustum::setCamera( Camera* cam )
{
	mCamera = cam;
}

float Frustum::getFov()
{
	return mFov;
}

float Frustum::getAspectRatio()
{
	return mAspect;
}

Matrix4 Frustum::getPerspective()
{
	updatePerspective();
	return mPerspective;
}

void Frustum::updatePerspective()
{
	float tany = tan( mFov * 0.5f);
	float tanx = tany * mAspect;

	float half_w = tanx * mNear;
	float half_h = tany * mNear;

	mLeft = -half_w;
	mRight = half_w;
	mBottom = -half_h;
	mTop = half_h;

	float invW = 1 / (mRight-mLeft);
	float invH = 1 / (mTop-mBottom);
	float invD = 1 / (mFar-mNear);

	float A = 2 * mNear * invW;
	float B = 2 * mNear * invH;
	float C = (mRight+mLeft) * invW;
	float D = (mTop+mBottom) * invH;

	float q, qn;
	
	q = - (mFar+mNear) * invD;
	qn = -2 * (mFar * mNear) * invD;

	mPerspective.zero();
	*mPerspective.m00 = A;
	*mPerspective.m20 = C;
	*mPerspective.m11 = B;
	*mPerspective.m21 = D;
	*mPerspective.m22 = q;
	*mPerspective.m32 = qn;
	*mPerspective.m23 = -1.f;
}

void Frustum::updatePlanes()
{
	Matrix4 clip = getPerspective() * mCamera->getMatrix();

	mPlanes[LEFT].normal.x = *clip.m30 + *clip.m00;
	mPlanes[LEFT].normal.y = *clip.m31 + *clip.m01;
	mPlanes[LEFT].normal.z = *clip.m32 + *clip.m02;
	mPlanes[LEFT].d = *clip.m33 + *clip.m03;

	mPlanes[RIGHT].normal.x = *clip.m30 - *clip.m00;
	mPlanes[RIGHT].normal.y = *clip.m31 - *clip.m01;
	mPlanes[RIGHT].normal.z = *clip.m32 - *clip.m02;
	mPlanes[RIGHT].d = *clip.m33 - *clip.m03;

	mPlanes[TOP].normal.x = *clip.m30 - *clip.m10;
	mPlanes[TOP].normal.y = *clip.m31 - *clip.m11;
	mPlanes[TOP].normal.z = *clip.m32 - *clip.m12;
	mPlanes[TOP].d = *clip.m33 - *clip.m13;

	mPlanes[BOTTOM].normal.x = *clip.m30 + *clip.m10;
	mPlanes[BOTTOM].normal.y = *clip.m31 + *clip.m11;
	mPlanes[BOTTOM].normal.z = *clip.m32 + *clip.m12;
	mPlanes[BOTTOM].d = *clip.m33 + *clip.m13;

	mPlanes[NEARP].normal.x = *clip.m30 + *clip.m20;
	mPlanes[NEARP].normal.y = *clip.m31 + *clip.m21;
	mPlanes[NEARP].normal.z = *clip.m32 + *clip.m22;
	mPlanes[NEARP].d = *clip.m33 + *clip.m23;

	mPlanes[FARP].normal.x = *clip.m30 - *clip.m20;
	mPlanes[FARP].normal.y = *clip.m31 - *clip.m21;
	mPlanes[FARP].normal.z = *clip.m32 - *clip.m22;
	mPlanes[FARP].d = *clip.m33 - *clip.m23;

	for(int p = 0; p < 6; p++)
	{
		mPlanes[p].normal = mPlanes[p].normal.normalize();
	}
}

int Frustum::intersectsAABB(const Vector3 &mins, const Vector3 &maxs)
{
	updatePlanes();
	Vector3 halfSize = Vector3(maxs) - Vector3(mins);
	Vector3 center = Vector3(mins) + halfSize;

	int ret = Frustum::INSIDE; 
	
	for(int p = 0; p < 6; p++)
	{
		if( p == FARP && mFar == 0 )
			continue; // Infinite view distance

		int side = mPlanes[p].getSide( center, halfSize );
		if( side == Plane::NEGATIVE )
			return Frustum::OUTSIDE;
	}

	return ret;
}