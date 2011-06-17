#include "Character.h"
#include "OpencraftCore.h"
#include "World.h"

Character::Character( void )
{
	setHeight( 1.9f );
	setMoveSpeed( 2.f );
	setSprintSpeed( 4.f );
	enableSprint( false );
	enableFlying( false );
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

void Character::enableFlying( bool f )
{
	mFlying = f;
}

bool Character::isFlying()
{
	return mFlying;
}

Camera* Character::getCamera()
{
	return &mCamera;
}

raycast_r Character::getEyeCast()
{
	raycast_r ray;
	ray.orig = getPosition() + Vector3( 0.f, mHeight * 0.9f, 0.f );
	ray.dir = mCamera.getForward();
	return ray;
}

raycast_r Character::getFeetCast()
{
	raycast_r ray;
	ray.orig = getPosition() + Vector3( 0.f, mHeight, 0.f );
	ray.dir = Vector3( 0.f, -1.f, 0.f );
	return ray;
}

collision_r Character::getCollision()
{
	collision_r col;
	col.min1 = getPosition() + Vector3( -.4f, 0.f, -.4f );
	col.max1 = getPosition() + Vector3( .4f, mHeight, .4f );
	return col;
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
	if( !mFlying )
	{
		raycast_r feetRay = getFeetCast();
		raycast_r gRay[4];
		// Perform 4 raycasts for edges of player since AABB collision testing doesn't work.
		feetRay.orig += Vector3(0.49f, 0.f, 0.49f);
		gRay[0] = OpencraftCore::Singleton->getWorld()->raycastWorld( feetRay, true );
		feetRay.orig += Vector3(0.f, 0.f, -0.98f);
		gRay[1] = OpencraftCore::Singleton->getWorld()->raycastWorld( feetRay, true );
		feetRay.orig += Vector3(-0.98f, 0.f, 0.f);
		gRay[2] = OpencraftCore::Singleton->getWorld()->raycastWorld( feetRay, true );
		feetRay.orig += Vector3(0.0f, 0.f, 0.98f);
		gRay[3] = OpencraftCore::Singleton->getWorld()->raycastWorld( feetRay, true );
		
		float i0 = -1;
		for( int i = 0; i < 4; i++ ) {
			if( gRay[i].hit == true ) {
				if( i0 = -1 ) {
					i0 = gRay[i].i0 - mHeight;
				}
				else
				{
					i0 = std::min<float>(i0, gRay[i].i0 - mHeight);
				}
			}
		}
		if( i0 != -1 )
			mPosition.y -= i0;
	}

	// Offset camera by eye height.
	mCamera.setPosition( mPosition + Vector3( 0.f, mHeight * 0.9f, 0.f) );
}