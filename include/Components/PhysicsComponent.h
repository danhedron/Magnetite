#ifndef _PHYSICSCOMPONENT_H_
#define _PHYSICSCOMPONENT_H_
#include "ComponentEvents.h"
#include <Component.h>
#include <LinearMath/btMotionState.h>
#include <BulletCollision/CollisionShapes/btConvexShape.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>

namespace Magnetite 
{
	
	class PhysicsComponent : public Component, public btMotionState
	{
	protected:
		btConvexShape*		mPhysicsShape;
		btRigidBody*		mPhysicsBody;
		btTransform			mWorld;
		float				mMass;
	public:
		
		PhysicsComponent(BaseEntity* ent);
		
		virtual ~PhysicsComponent();
		
		void create();
		
		virtual void event( const BaseEvent& event );
		
		virtual void think( float dt );
		
		void setTransform( const btTransform& t );
		
		void setMass( float m );
		
		void getWorldTransform( btTransform& world ) const;

		void setWorldTransform( const btTransform &world );
		
		virtual Magnetite::String getType() { return "physics"; }
	};
	
};

#endif
