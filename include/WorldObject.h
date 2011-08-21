#ifndef _WORLDOBJECT_H_
#define _WORLDOBJECT_H_
#include "prerequisites.h"

class WorldObject
{
protected:
	Vector3 mPosition;
	
public:
	WorldObject();
	~WorldObject();

	/**
	 * Set the object's world position
	 */
	virtual void setPosition( const Vector3 &v );

	/**
	 * Get the object's world position
	 */
	Vector3 getPosition( );

	/**
	 * Lets this object do any thinking it needs to do.
	 * @param dt delta time
	 */
	virtual void update( float dt );

};

#endif //