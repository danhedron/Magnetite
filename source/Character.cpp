#include "Character.h"

Character::Character( void )
{
	setHeight( 1.9f );
	setMoveSpeed( 1.2f );
	setSprintSpeed( 3.f );
	enableSprint( false );
}

Character::~Character( void )
{

}

void Character::setSkin(std::string name)
{
	Util::log("Setting skin to: " + name);
}

void Character::setHeight( float h )
{
	mHeight = h;
}

void Character::setMoveSpeed( float s )
{
	mMoveSpeed = s;
}

void Character::setSprintSpeed( float s )
{
	mSprintSpeed = s;
}

void Character::enableSprint( bool s )
{
	mSprint = s;
}

Camera* Character::getCamera()
{
	return &mCamera;
}

raycast_r Character::getEyeCast()
{
	raycast_r ray;
	ray.orig = getPosition();
	ray.dir = mCamera.getForward();
	return ray;
}

void Character::addMoveDelta(const Vector3 &dp)
{
	mMoveVector += dp;
}

void Character::setPosition(const Vector3 &vec)
{
	mPosition = vec;
}

Vector3 Character::getPosition()
{
	return mPosition;
}

void Character::update(float dt)
{
	mPosition += ( Matrix4::rotateY( (mCamera.getYaw()*3.141f)/180 ) * mMoveVector ) * ( mSprint ? mSprintSpeed : mMoveSpeed ) * dt;
	// Offset camera by eye height.
	mCamera.setPosition( mPosition + Vector3( 0.f, mHeight * 0.9f, 0.f) );
}