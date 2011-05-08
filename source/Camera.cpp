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
	Matrix4 mat2;
	mat2.rotateY( mYaw*(3.141f/180) );
	Matrix4 mat;
	mat.translate( -mPosition );
	mat = mat2*mat;
	return mat;
}

void Camera::applyMatrix() 
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//Util::log( Util::toString( getMatrix().getTranslation() ) );
	//glMultMatrixf( getMatrix().matrix );
	glRotatef( mPitch, 1, 0, 0);
	glRotatef( mYaw, 0, 1, 0);
	glTranslatef( -mPosition.x, -mPosition.y, -mPosition.z );
}

void Camera::setPosition(Vector3 &v)
{
	mPosition = v;
}

Vector3 Camera::getPosition()
{
	return mPosition;
}

void Camera::yaw( float amt )
{
	mYaw += amt;
}

void Camera::pitch( float amt )
{
	mPitch += amt;
}

void Camera::translate( Vector3& vec )
{
	Vector3 v = getPosition();
	
	vec.x = (vec.x * cos(mYaw*(3.141f/180))) - (vec.z * sin(mYaw*(3.141f/180)));
	vec.z = (vec.z * cos(mYaw*(3.141f/180))) + (vec.x * sin(mYaw*(3.141f/180)));

	vec.z = (vec.z * cos(mPitch*(3.141f/180))) - (vec.y * sin(mPitch*(3.141f/180)));
	vec.y = (vec.y * cos(mPitch*(3.141f/180))) + (vec.z * sin(mPitch*(3.141f/180)));

	Util::log( Util::toString( vec.y ) );
	setPosition( Vector3( vec.x + v.x, vec.y + v.y, vec.z + v.z ) );
}