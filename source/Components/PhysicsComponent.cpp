#include "Components/PhysicsComponent.h"
#include <util.h>
#include <BaseEntity.h>
#include <MagnetiteCore.h>

namespace Magnetite 
{
	
	PhysicsComponent::PhysicsComponent( BaseEntity* ent )
	: Component(ent),
	mPhysicsShape(NULL),
	mPhysicsBody(NULL),
	mWorld(btTransform( btQuaternion(), btVector3( 0, 0, 0 ) )),
	mMass( 1.f )
	{
	}
	
	PhysicsComponent::~PhysicsComponent()
	{
		if( mPhysicsBody != NULL )
		{
			MagnetiteCore::Singleton->getPhysicsWorld()->removeRigidBody(mPhysicsBody);
			delete mPhysicsBody;
		}
		
		if( mPhysicsShape != NULL )
		{
			delete mPhysicsShape;
		}
	}
	
	void PhysicsComponent::create()
	{
		if( mPhysicsShape == nullptr )
		{
			mPhysicsShape = new btSphereShape( 1.f );
			btVector3 inertia(0,0,0);
			if(mMass > 0.f)
			{
				mPhysicsShape->calculateLocalInertia( mMass, inertia );
			}

			btRigidBody::btRigidBodyConstructionInfo ci( mMass, this, mPhysicsShape, inertia );
			mPhysicsBody = new btRigidBody( ci );
			//mPhysicsBody->setActivationState(DISABLE_DEACTIVATION);
			MagnetiteCore::Singleton->getPhysicsWorld()->addRigidBody( mPhysicsBody );
		}
	}
	
	void PhysicsComponent::event( const BaseEvent& ev )
	{
		if( ev.eventID == CE_POSITION_UPDATED )
		{
			MovementEvent* mv = (MovementEvent*)&ev;
			mPhysicsBody->getWorldTransform().setOrigin( btVector3( mv->position.x, mv->position.y, mv->position.z ) );
		}
	}
	
	void PhysicsComponent::think( float dt )
	{
	}
	
	void PhysicsComponent::setTransform( const btTransform& t )
	{
		mWorld = t;
		setWorldTransform(t);
	}
	
	void PhysicsComponent::setMass( float m )
	{
		mMass = m;
	}
	
	void PhysicsComponent::getWorldTransform( btTransform& world ) const {
		world = mWorld;
	}
	
	void PhysicsComponent::setWorldTransform( const btTransform &world ) {
		
		btQuaternion rot = world.getRotation();
		btVector3 p = world.getOrigin();
		
		MovementEvent me;
		me.setDefaults();
		
		me.source = this;
		me.eventID = CE_POSITION_UPDATED;
		me.position = Vector3( p.x(), p.y(), p.z() );
		
		mEntity->fireEvent(me);;
	}
	
};
