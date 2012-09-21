#include "Camera.h"
#include "World.h"

#include "Matrix.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtx/transform.hpp>


Camera::Camera( World* world )
: PagingCamera( world ),
mYaw( 0 ),
mPitch ( 0 )
{
	mViewFrustum.setCamera(this);
	setPosition(Vector3());
}

Camera::~Camera(void)
{
	//PagingCamera::~PagingCamera();
}

glm::mat4 Camera::getMatrix()
{
	glm::mat4 mat;
	mat = glm::translate( mat, mPosition );
	
	mat = glm::rotate(mat, getYaw(), Vector3(0.f, 1.f, 0.f));
	mat = glm::rotate(mat, getPitch(), Vector3(1.f, 0.f, 0.f));

	return mat;
}

glm::mat3 Camera::getOrientationMatrix()
{
	glm::mat4 mat;
	
	mat = glm::rotate(mat, getYaw(), Vector3(0.f, 1.f, 0.f));
	mat = glm::rotate(mat, getPitch(), Vector3(1.f, 0.f, 0.f));

	return glm::mat3( mat );
}

Frustum& Camera::getFrustum()
{
	return mViewFrustum;
}

Vector3 Camera::getForward() 
{
	return glm::normalize( getOrientationMatrix() * glm::vec3( 0.f, 0.f, -1.f ) );
}

void Camera::applyMatrix( bool rot, bool pos ) 
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultMatrixf( glm::value_ptr( glm::inverse(getMatrix()) ) );
}

void Camera::applyModelViewMatrix( bool rot, bool pos ) 
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if( rot ) {
		glRotatef( mPitch, 1, 0, 0);
		glRotatef( mYaw, 0, 1, 0);
	}
	if( pos ) {
		glTranslatef( mPosition.x, mPosition.y, mPosition.z );
	}
}

void Camera::setPosition(const Vector3 &v)
{
	mPosition = v;
	mViewFrustum.updatePlanes();
	PagingCamera::setPosition( v );
}

Vector3 Camera::getPosition()
{
	return mPosition;
}

void Camera::yaw( float amt )
{
	setYaw( mYaw + amt );
}

void Camera::pitch( float amt ) 
{
	setPitch( mPitch + amt );
}

void Camera::setPitch( float p )
{
	mPitch = std::max<float>( std::min<float>( p, 90 ), -90 );
	mViewFrustum.updatePlanes();
}

void Camera::setYaw( float y )
{
	mYaw = y;
	while( mYaw > 360 )
		mYaw -= 360;
	while( mYaw < -360 )
		mYaw += 360;
	mViewFrustum.updatePlanes();
}

float Camera::getPitch()
{
	return mPitch;
}

float Camera::getYaw() 
{
	return mYaw;
}

void Camera::translate(const Vector3& vec )
{
	Vector3 pv = getPosition();
	
	auto r = getOrientationMatrix();
	
	glm::vec3 f = r * vec;
	
	setPosition( Vector3( f.x + pv.x, f.y + pv.y, f.z + pv.z ) );
}
