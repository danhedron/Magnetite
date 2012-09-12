#include "Frustum.h"
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

float Plane::distance( const Vector3& p )
{
	return ( glm::dot( normal, p )  + d);
}

int Plane::getSide( const Vector3& center, const Vector3& halfSize )
{
	float dist = distance(center);

	float maxDist = abs( glm::dot( normal, halfSize ) );
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
mLeft( 0 ),
mRight( 0 ),
mTop( 0 ),
mBottom( 0 ),
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
	mFov = fov;
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
	mPerspective = glm::perspective( mFov, getAspectRatio(), mNear, mFar );
}

void Frustum::updatePlanes()
{
	Matrix4 view =  mCamera->getMatrix();
	Matrix4 clip = getPerspective() * view;
	
	// todo: this.

}

int Frustum::intersectsAABB(const Vector3 &mins, const Vector3 &maxs)
{
	updatePlanes();
	Vector3 halfSize = Vector3(maxs) - Vector3(mins);
	Vector3 center = Vector3(mins) + halfSize;

	/*if( Vector3(maxs) < Vector3(mins) ) {
		Util::log("Error: Negative volume in aabb");
	}*/

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
	/*if( mFrustumVolume )
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

	*/	
}