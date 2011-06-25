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
mCamera( NULL ),
mFrustumVolume( NULL )
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
	mPerspective.m[0][0] = A;
	mPerspective.m[2][0] = C;
	mPerspective.m[1][1] = B;
	mPerspective.m[2][1] = D;
	mPerspective.m[2][2] = q;
	mPerspective.m[3][2] = qn;
	mPerspective.m[2][3] = -1.f;
}

void Frustum::updatePlanes()
{
	Matrix4 view =  mCamera->getMatrix();
	Matrix4 clip = getPerspective() * view;

	mPlanes[LEFT].normal.x = clip.m[0][3] + clip.m[0][0];
	mPlanes[LEFT].normal.y = clip.m[1][3] + clip.m[1][0];
	mPlanes[LEFT].normal.z = clip.m[2][3] + clip.m[2][0];
	mPlanes[LEFT].d = clip.m[3][3] + clip.m[3][0];

	mPlanes[RIGHT].normal.x = clip.m[0][3] - clip.m[0][0];
	mPlanes[RIGHT].normal.y = clip.m[1][3] - clip.m[1][0];
	mPlanes[RIGHT].normal.z = clip.m[2][3] - clip.m[2][0];
	mPlanes[RIGHT].d = clip.m[3][3] - clip.m[3][0];

	mPlanes[TOP].normal.x = clip.m[0][3] - clip.m[0][1];
	mPlanes[TOP].normal.y = clip.m[1][3] - clip.m[1][1];
	mPlanes[TOP].normal.z = clip.m[2][3] - clip.m[2][1];
	mPlanes[TOP].d = clip.m[3][3] - clip.m[3][1];

	mPlanes[BOTTOM].normal.x = clip.m[0][3] + clip.m[0][1];
	mPlanes[BOTTOM].normal.y = clip.m[1][3] + clip.m[1][1];
	mPlanes[BOTTOM].normal.z = clip.m[2][3] + clip.m[2][1];
	mPlanes[BOTTOM].d = clip.m[3][3] + clip.m[3][1];

	mPlanes[NEARP].normal.x = clip.m[0][3] + clip.m[0][2];
	mPlanes[NEARP].normal.y = clip.m[1][3] + clip.m[1][2];
	mPlanes[NEARP].normal.z = clip.m[2][3] + clip.m[2][2];
	mPlanes[NEARP].d = clip.m[3][3] + clip.m[3][2];

	mPlanes[FARP].normal.x = clip.m[0][3] - clip.m[0][2];
	mPlanes[FARP].normal.y = clip.m[1][3] - clip.m[1][2];
	mPlanes[FARP].normal.z = clip.m[2][3] - clip.m[2][2];
	mPlanes[FARP].d = clip.m[3][3] - clip.m[3][2];

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

	if(  Vector3(maxs) < Vector3(mins) ) {
		Util::log("Error: Negative volume in aabb");
	}

	int ret = Frustum::INSIDE;
	
	for(int p = 0; p < 6; p++)
	{
		if( p == FARP && mFar == 0 )
			continue; // Infinite view distance

		int side = mPlanes[p].getSide( center, halfSize );
		if( side == Plane::NEGATIVE ) {
			return Frustum::OUTSIDE;
		}
		else if( side == Plane::BOTH ) {
			ret = Frustum::INTERSECTS;
		}
	}
	
	return ret;
}

void Frustum::updateFrustumVolume()
{
	if( mFrustumVolume )
		delete mFrustumVolume;
	mFrustumVolume = new GLgeometry;

	float radio = mFar / mNear;

	float farLeft = mLeft * radio;
	float farRight = mRight * radio;
	float farBottom = mBottom * radio;
	float farTop = mTop * radio;

	GLvertex* verts = new GLvertex[24];
	GLedge* edges = new GLedge[24];
	verts[ 0] = Renderer::vertex( mLeft, mTop, -1.f );
	edges[ 0] = 0;
	verts[ 1] = Renderer::vertex( mRight, mTop, -1.f );
	edges[ 1] = 1;

	verts[ 2] = Renderer::vertex( mRight, mTop, -1.f );
	edges[ 2] = 2;
	verts[ 3] = Renderer::vertex( mRight, mBottom, -1.f );
	edges[ 3] = 3;

	verts[ 4] = Renderer::vertex( mRight, mBottom, -1.f );
	edges[ 4] = 3;
	verts[ 5] = Renderer::vertex( mLeft, mBottom, -1.f );
	edges[ 5] = 5;

	verts[ 6] = Renderer::vertex( mLeft, mBottom, -1.f );
	edges[ 6] = 6;
	verts[ 7] = Renderer::vertex( mLeft, mTop, -1.f );
	edges[ 7] = 7;

	//---

	verts[ 8] = Renderer::vertex( mLeft, farTop, -mFar );
	edges[ 8] = 8;
	verts[ 9] = Renderer::vertex( mRight, farTop, -mFar );
	edges[ 8] = 9;
	
	verts[10] = Renderer::vertex( mRight, farTop, -mFar );
	edges[10] = 10;
	verts[11] = Renderer::vertex( mRight, farBottom, -mFar );
	edges[11] = 11;

	verts[12] = Renderer::vertex( mRight, farBottom, -mFar );
	edges[12] = 12;
	verts[13] = Renderer::vertex( mLeft, farBottom, -mFar );
	edges[13] = 13;

	verts[14] = Renderer::vertex( mLeft, farBottom, -mFar );
	edges[14] = 14;
	verts[15] = Renderer::vertex( mLeft, farTop, -mFar );
	edges[15] = 15;

	//---

	verts[16] = Renderer::vertex( 0.f, 0.f, 0.f );
	edges[16] = 16;
	verts[17] = Renderer::vertex( mLeft, mTop, -mNear );
	edges[17] = 17;
	
	verts[18] = Renderer::vertex( 0.f, 0.f, 0.f );
	edges[18] = 18;
	verts[19] = Renderer::vertex( mRight, mTop, -mNear );
	edges[19] = 19;

	verts[20] = Renderer::vertex( 0.f, 0.f, 0.f );
	edges[20] = 20;
	verts[21] = Renderer::vertex( mRight, mBottom, -mNear );
	edges[21] = 21;

	verts[22] = Renderer::vertex( 0.f, 0.f, 0.f );
	edges[22] = 22;
	verts[23] = Renderer::vertex( mLeft, mBottom, -mNear );
	edges[23] = 23;

	mFrustumVolume->vertexData = verts;
	mFrustumVolume->vertexCount = 24;
	mFrustumVolume->edgeData = edges;
	mFrustumVolume->edgeCount = 24;

		
}