#include "Components/PhysicsComponent.h"
#include <util.h>
#include <BaseEntity.h>
#include <MagnetiteCore.h>

namespace Magnetite 
{
	
	PhysicsComponent::PhysicsComponent( BaseEntity* ent )
	: Component(ent),
	mPhysicsShape(NULL),
	mPhysicsBody(NULL)
	{
		mWorld = btTransform( btQuaternion(), btVector3( 0, 300.f, 0 ) );
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
		mPhysicsShape = new btBoxShape( btVector3(0.5, 0.5, 0.5 ) );
		btScalar mass = 1;
		btVector3 inertia(0,0,0);
		mPhysicsShape->calculateLocalInertia( mass, inertia );

		btRigidBody::btRigidBodyConstructionInfo ci( mass, this, mPhysicsShape, inertia );
		mPhysicsBody = new btRigidBody( ci );
		//mPhysicsBody->setActivationState(DISABLE_DEACTIVATION);
		MagnetiteCore::Singleton->getPhysicsWorld()->addRigidBody( mPhysicsBody );
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
		if( mPhysicsBody )
			mPhysicsBody->activate(true);
	}
	
	void PhysicsComponent::getWorldTransform( btTransform& world ) const {
		world = mWorld;
	}
	
	void PhysicsComponent::setWorldTransform( const btTransform &world ) {
		//mWorld = world;
		
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
