#include "Character.h"
#include "MagnetiteCore.h"
#include "World.h"
#include "PhysicsState.h"

Character::Character( void ) :
mPhysicsController( NULL )
{
	setHeight( 1.9f );
	setMoveSpeed( 2.f );
	setSprintSpeed( 4.f );
	enableSprint( false );
	enableFlying( false );
	this->_initPhysics();
}

Character::~Character( void )
{
	PhysicsWorldObject::~PhysicsWorldObject();
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

void Character::_initPhysics()
{
	//btTransform spawn;
	//spawn.setIdentity();
	//spawn.setOrigin( btVector3( 0, 150, 0 ) );
	//mPhysicsState = new PhysicsState( spawn, this );
	mPhysicsShape = new btCapsuleShape( 0.45, mHeight );//btBoxShape( btVector3( 4, 4, 4 ) );//
	
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
	//Matrix4 yaw = Matrix4::rotateY( -(mCamera.getYaw()*3.141f)/180 );

	if( !mFlying ) {
		Vector3 walkDir = Matrix4::rotateY( -(mCamera.getYaw()*3.141f)/180) * mMoveVector * (mSprint ? mSprintSpeed : mMoveSpeed) * dt;
		mPhysicsController->setWalkDirection( btVector3( walkDir.x, walkDir.y, walkDir.z ) );
		
		btTransform t = mPhysicsBody->getWorldTransform();
		mPosition = Vector3( t.getOrigin().x(), t.getOrigin().y(), t.getOrigin().z() );

	}
	else
	{
		Vector3 delty = Matrix4::rotateX( -(mCamera.getPitch()*3.141f)/180) * -mMoveVector * 5;
		Vector3 delt = Matrix4::rotateY( -(mCamera.getYaw()*3.141f)/180) * delty * 5;
		mPosition -= delt * dt;
	}

	mCamera.setPosition( mPosition + Vector3( 0.f, (mHeight/2) * 0.9f, 0.f) );
}

void Character::_physicsUpdateTransform( const Vector3& v )
{
	if( !mFlying )
		mPosition = v;
}