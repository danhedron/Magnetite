#include "Camera.h"
#include "Matrix.h"

Camera::Camera(void)
: mPitch ( 0 ),
mYaw( 0 )
{
}

Camera::~Camera(void)
{

}

Matrix4 Camera::getMatrix()
{
	Matrix4 matX = Matrix4::rotateX( (mPitch*3.141f)/180 );
	Matrix4 matY = Matrix4::rotateY( (mYaw*3.141f)/180 - (3.141f) );
	Matrix4 rotated = matY * matX;

	return rotated;
}

void Camera::applyMatrix() 
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef( -mPitch, 1, 0, 0);
	glRotatef( -mYaw, 0, 1, 0);
	glTranslatef( -mPosition.x, -mPosition.y, -mPosition.z );
}

void Camera::setPosition(const Vector3 &v)
{
	mPosition = v;
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
}

void Camera::setYaw( float y )
{
	mYaw = y;
}

void Camera::translate(const Vector3& vec )
{
	Vector3 pv = getPosition();
	Vector3 f;
	
	//matrix = Matrix4();

	Matrix4 matX = Matrix4::rotateX( (mPitch*3.141f)/180 );
	Matrix4 matY = Matrix4::rotateY( (mYaw*3.141f)/180 );
	f = matX * matY * vec;

	setPosition( Vector3( f.x + pv.x, f.y + pv.y, f.z + pv.z ) );
}