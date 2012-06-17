#ifndef _CHARACTER_H_
#define _CHARACTER_H_
#include "prerequisites.h"
#include "Camera.h"
#include "World.h"
#include "PhysicsWorldObject.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

class Character : public PhysicsWorldObject
{
protected:
	/**
	 * Move vector for the movements
	 */
	Vector3 mMoveVector;
	
	/**
	 * Is the character currently experiencing physics.
	 */
	bool mFlying;
	
	/**
	 * Movement speed
	 */
	float mMoveSpeed;
	
	/**
	 * Sprinting speed
	 */
	float mSprintSpeed;
	
	/**
	 * Are we moving quicker.
	 */
	bool mSprint;
	
	/**
	 * Camera for look angles.
	 */
	Camera mCamera;
	
	/**
	 * Offset of the camera is 3/4ths of this value from the position.
	 */
	float mHeight;

	btKinematicCharacterController* mPhysicsController;
	btPairCachingGhostObject*		mPhysicsBody;

	virtual void _initPhysics();
public:
	
	Character( );
	virtual ~Character( );

	/**
	 * Sets the player's texture to another.
	 * @param name Texture ID
	 */
	void setSkin( std::string name );

	/**
	 * Sets the height of the player
	 */
	void setHeight( float h );

	/**
	 * Sets the character's move speed
	 */
	void setMoveSpeed( float s );

	/**
	 * Sets the character's sprint speed
	 */
	void setSprintSpeed( float s );

	/**
	 * switch betweeen sprinting and walking
	 */
	void enableSprint( bool s );

	/**
	 * Enables/disables flying
	 */
	void enableFlying( bool f );

	/**
	 * Returns if the character is currently flying
	 */
	bool isFlying();

	/**
	 * Gets a pointer to the camera
	 */
	Camera* getCamera();
	
	/**
	 * Gets a ray representing the Player's look
	 */
	raycast_r getEyeCast();
	
	/**
	 * Gets the direction the player is looking in
	 */
	Vector3 getLookDirection();
	
	/**
	 * Gets a ray representing the -UP axis
	 */
	raycast_r getFeetCast();

	/**
	 * Gets the collision test for this character
	 */
	void getCollision( Vector3& min, Vector3& max );

	/**
	 * Adds move delta to character's move delta
	 * @param dp Position delta
	 */
	void addMoveDelta( const Vector3& dp );
	
	/**
	 * Moves the character by the given delta.
	 */
	void move( const Vector3& v );

	/**
	 * Update camera poisition and stuff.
	 * @param dt delta time
	 */
	virtual void update( float dt );

	/**
	 * Jump :- todo: more options
	 */
	virtual void jump();

	/**
	 * Set position
	 */
	virtual void setPosition( const Vector3& v );

	/**
	 * Callback for physics - overriden for flying magic
	 */
	virtual void _physicsUpdateTransform( const Vector3& v );

};

#endif
