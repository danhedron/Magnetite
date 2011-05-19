#ifndef _CHARACTER_H_
#define _CHARACTER_H_
#include "prerequisites.h"
#include "Camera.h"
#include "World.h"

class Character
{
protected:
	Vector3 mPosition;
	Vector3 mMoveVector;
	Vector3 mGravity;
	float mMoveSpeed;
	float mSprintSpeed;
	bool mSprint;
	Camera mCamera; //< Camera angles used for character rotation.
	float mHeight; //< Height of the player in world units.
public:
	
	Character( void );
	~Character( void );

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
	 * Gets a pointer to the camera
	 */
	Camera* getCamera();

	/**
	 * Gets a ray representing the Character's look
	 */
	raycast_r getEyeCast();
	
	/**
	 * Gets a ray representing the -UP axis
	 */
	raycast_r getFeetCast();

	/**
	 * Gets the collision test for this character
	 */
	collision_r getCollision();

	/**
	 * Sets the characer's position immediatley.
	 * @param vec World coordinates.
	 */
	void setPosition( const Vector3& vec );

	/**
	 * Returns the player's world coordinates
	 */
	Vector3 getPosition();

	/**
	 * Adds move delta to character's move delta
	 * @param dp Position delta
	 */
	void addMoveDelta( const Vector3& dp );

	/**
	 * Updates the position and other gubbins.
	 * @param dt delta time
	 */
	void update( float dt );

};

#endif
