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
	Vector3 pv = getPosition();
	Vector3 f;
	
	Matrix4 matrix = Matrix4();
	matrix.translate( vec );

	matrix = matrix * Matrix4::rotateY( mYaw*(3.141f/180) );

	f = vec;

	//Util::log( Util::toString( vec.y ) );
	setPosition( Vector3( f.x + pv.x, f.y + pv.y, f.z + pv.z ) );
}