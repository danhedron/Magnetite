#include "PhysicsWorldObject.h"
#include "PhysicsState.h"
#include "OpencraftCore.h"

PhysicsWorldObject::PhysicsWorldObject() :
	mPhysicsState( NULL ),
	mPhysicsBody( NULL )
{

}

PhysicsWorldObject::~PhysicsWorldObject() 
{
	delete mPhysicsShape;
	delete mPhysicsBody;
	delete mPhysicsState;
}

void PhysicsWorldObject::setPosition( const Vector3 &v )
{
	mPosition = v;
	if( mPhysicsBody != NULL ) {
		btTransform t = mPhysicsBody->getCenterOfMassTransform();
		t.setOrigin( btVector3( v.x, v.y, v.z ) );
		mPhysicsBody->setCenterOfMassTransform( t );
	}
}

void PhysicsWorldObject::_initPhysics()
{
	Util::log("Default Physics Constructor");
	mPhysicsState = new PhysicsState( btTransform(), this );
	mPhysicsShape = new btBoxShape( btVector3(0.5, 0.5, 0.5 ) );
	btScalar mass = 1;
	btVector3 inertia(0,0,0);
	mPhysicsShape->calculateLocalInertia( mass, inertia );

	btRigidBody::btRigidBodyConstructionInfo ci( mass, mPhysicsState, mPhysicsShape, inertia );
	mPhysicsBody = new btRigidBody( ci );
	//mPhysicsBody->setActivationState(DISABLE_DEACTIVATION);
	OpencraftCore::Singleton->getPhysicsWorld()->addRigidBody( mPhysicsBody );
}

void PhysicsWorldObject::_physicsUpdateTransform( const Vector3 &v )
{
	mPosition = v;
}