#include "Player.h"

Player::Player()
{
	mPosition = Vector3( 0, 0, 0 );
}

Player::~Player()
{
	
}

Camera& Player::getCamera()
{
	return mCamera;
}

raycast_r Player::getEyeCast()
{
	raycast_r ray;
	ray.orig = getPosition();
	ray.dir = getLookDirection();
	return ray;
} 

Vector3 Player::getLookDirection()
{
	return mCamera.getForward();
}

void Player::move( const Vector3& offset )
{
	mCamera.translate( offset );
}

void Player::setPosition( const Vector3& pos )
{
	mCamera.setPosition( pos );
}

Vector3 Player::getPosition()
{
	return mCamera.getPosition();
}