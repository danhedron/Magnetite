#include "Camera.h"
#include "Matrix.h"

Camera::Camera(void)
: mPitch ( 0 ),
mYaw( 0 )
{
	mViewFrustum.setCamera(this);
	setPosition(Vector3());
}

Camera::~Camera(void)
{

}

Matrix4 Camera::getMatrix()
{
	Matrix4 mat;
	Matrix4 yaw = Matrix4::rotateY( 3.141f-(getYaw()*3.141f)/180 );
	Matrix4 pitch = Matrix4::rotateX( (getPitch()*3.141f)/180 );
	Matrix4 rot = yaw;//(pitch * yaw);

	rot.translate( mPosition );
	return mat;
}

Matrix4 Camera::getOrientationMatrix()
{
	Matrix4 yaw = Matrix4::rotateY( 3.141f-(getYaw()*3.141f)/180 );
	Matrix4 pitch = Matrix4::rotateX( (getPitch()*3.141f)/180 );
	Matrix4 rotated = (pitch * yaw);

	return rotated;
}

Frustum& Camera::getFrustum()
{
	return mViewFrustum;
}

Vector3 Camera::getForward() 
{
	Matrix4 yaw = Matrix4::rotateY( 3.141f-(getYaw()*3.141f)/180 );
	Matrix4 pitch = Matrix4::rotateX( (getPitch()*3.141f)/180 );
	
	Vector3 vec = Vector3(0.f, 0.f, 1.f);
	vec = pitch * vec;
	vec = yaw * vec;
	return vec.normalize();
}

void Camera::applyMatrix( bool rot, bool pos ) 
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if( rot ) {
		glRotatef( -mPitch, 1, 0, 0);
		glRotatef( -mYaw, 0, 1, 0);
	}
	if( pos ) {
		glTranslatef( -mPosition.x, -mPosition.y, -mPosition.z );
	}
}

void Camera::setPosition(const Vector3 &v)
{
	mPosition = v;
	mViewFrustum.updatePlanes();
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
	Vector3 f;
	
	//matrix = Matrix4();

	Matrix4 matX = Matrix4::rotateX( (mPitch*3.141f)/180 );
	Matrix4 matY = Matrix4::rotateY( 3.141f/2 - (mYaw*3.141f)/180 );
	f = matY * vec;

	setPosition( Vector3( f.x + pv.x, f.y + pv.y, f.z + pv.z ) );
}