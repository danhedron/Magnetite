#include "Character.h"
#include "MagnetiteCore.h"
#include "World.h"
#include "PhysicsState.h"
#include <glm/gtc/matrix_transform.hpp>

Character::Character( ) 
: mCamera( MagnetiteCore::Singleton->getWorld() ),
mPhysicsController( NULL )
{
	setHeight( 1.9f );
	setMoveSpeed( 2.f );
	setSprintSpeed( 8.f );
	enableSprint( false );
	enableFlying( false );
	this->_initPhysics();
}

Character::~Character( )
{
	//PhysicsWorldObject::~PhysicsWorldObject();
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
	ray.orig = getPosition() + Vector3( 0.f, mHeight /2, 0.f ) + mCamera.getForward();
	ray.dir = mCamera.getForward();
	return ray;
} 

Vector3 Character::getLookDirection()
{
	return mCamera.getForward();
}

raycast_r Character::getFeetCast()
{
	raycast_r ray;
	ray.orig = getPosition() + Vector3( 0.f, mHeight, 0.f );
	ray.dir = Vector3( 0.f, -1.f, 0.f );
	return ray;
}

void Character::_initPhysics()
{
	//btTransform spawn;
	//spawn.setIdentity();
	//spawn.setOrigin( btVector3( 0, 150, 0 ) );
	//mPhysicsState = new PhysicsState( spawn, this );
	mPhysicsShape = new btCapsuleShape( 0.45, mHeight - ( 0.45 * 2 ) );//btBoxShape( btVector3( 4, 4, 4 ) );//
	
	//btRigidBody::btRigidBodyConstructionInfo ci( mass, mPhysicsState, mPhysicsShape, inertia );
	mPhysicsBody = new btPairCachingGhostObject();
	mPhysicsBody->setCollisionShape( mPhysicsShape );
	mPhysicsBody->setCollisionFlags( mPhysicsBody->getCollisionFlags() | btCollisionObject::CF_CHARACTER_OBJECT);

	mPhysicsController = new btKinematicCharacterController(mPhysicsBody, mPhysicsShape, 0.5);
	MagnetiteCore::Singleton->getPhysicsWorld()->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	MagnetiteCore::Singleton->getPhysicsWorld()->addCollisionObject(mPhysicsBody,btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter|btBroadphaseProxy::DefaultFilter | btBroadphaseProxy::AllFilter);
	MagnetiteCore::Singleton->getPhysicsWorld()->addAction( mPhysicsController );
}

void Character::addMoveDelta(const Vector3 &dp)
{
	mMoveVector += dp;
}

void Character::setPosition( const Vector3& v )
{
	btTransform t = mPhysicsBody->getWorldTransform();
	t.setOrigin( btVector3( v.x, v.y, v.z ) );
	mPhysicsBody->setWorldTransform( t );
}

void Character::jump()
{
	mPhysicsController->jump();
}

void Character::update(float dt)
{
	//move( mMoveVector * dt );
	
	btTransform t = mPhysicsBody->getWorldTransform();
	mPosition = Vector3( t.getOrigin().x(), t.getOrigin().y(), t.getOrigin().z() );
	
	if( mFlying ) mPhysicsController->setGravity(0);
	else mPhysicsController->setGravity(1);
	
	mCamera.setPosition( mPosition + Vector3( 0.f, (mHeight/2) * 0.9f, 0.f) );
}

void Character::move( const Vector3& v )
{
	// translate the vector around the yaw.
	auto r = glm::rotate( glm::mat4(), mCamera.getYaw(), glm::vec3(0.f, 1.f, 0.f)) ;
	Vector3 final = (glm::mat3(r) * v) * (mSprint ? mSprintSpeed : mMoveSpeed);
	
	mPhysicsController->setWalkDirection( btVector3( final.x, final.y, final.z ) );
		
	btTransform t = mPhysicsBody->getWorldTransform();
	mPosition = Vector3( t.getOrigin().x(), t.getOrigin().y(), t.getOrigin().z() );

}

void Character::_physicsUpdateTransform( const Vector3& v )
{
	if( !mFlying )
		mPosition = v;
}
