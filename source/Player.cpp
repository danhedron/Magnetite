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
	ray.dir = mCamera.getForward();
	return ray;
} 

void Player::setPosition( const Vector3& pos )
{
	mCamera.setPosition( pos );
}

Vector3 Player::getPosition()
{
	return mCamera.getPosition();
}