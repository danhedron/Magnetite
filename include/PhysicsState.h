#ifndef _PHYSICSSTATE_H_
#define _PHYSICSSTATE_H_
#include "prerequisites.h"
#include "PhysicsWorldObject.h"

//class PhysicsWorldObject;

class PhysicsState : public btMotionState
{
public:
	PhysicsState( const btTransform& initialTrans, PhysicsWorldObject* obj ) :
		mObject( NULL )
	{
		mPos = initialTrans;
		mObject = obj;
	}

	void getWorldTransform( btTransform& world ) const {
		world = mPos;
	}

	void setWorldTransform( const btTransform &world ) {
		if( mObject ==  NULL ) return;
		//btQuaternion rot = world.getRotation();
		// Wait we don't support that yet.
		btVector3 p = world.getOrigin();
		mObject->_physicsUpdateTransform( Vector3( p.x(), p.y(), p.z() ) );
	}

protected:
	PhysicsWorldObject* mObject;
	btTransform mPos;
};

#endif //