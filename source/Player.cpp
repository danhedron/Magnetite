#include "Player.h"
#include <Character.h>
#include <MagnetiteCore.h>

Player::Player()
{
	
}

Player::~Player()
{
	
}

void Player::move( const Vector3& offset )
{
	mCamera.translate( offset );
}