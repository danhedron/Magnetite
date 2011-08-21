#ifndef _PHYSICSWORLDOBJECT_H_
#define _PHYSICSWORLDOBJECT_H_
#include "prerequisites.h"
#include "WorldObject.h"

class PhysicsState;

class PhysicsWorldObject : public WorldObject
{
protected:
	PhysicsState*		mPhysicsState;
	btConvexShape*	mPhysicsShape;
	btRigidBody*		mPhysicsBody;

	/** 
	 * Initializes our internal physics objects
	 */
	virtual void _initPhysics();
	
public:
	PhysicsWorldObject();
	~PhysicsWorldObject();

	virtual void setPosition( const Vector3 &v );
	
	/**
	 * Physics callback function
	 */
	virtual void _physicsUpdateTransform( const Vector3& v );
};

#endif //