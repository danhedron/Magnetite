#ifndef _PLAYER_H_
#define _PLAYER_H_
#include "prerequisites.h"
#include "Camera.h"
#include "World.h"
#include "Character.h"

class Player : public Character
{
public:
	
	Player();
	~Player();
	
	/**
	 * Adjusts the player's position by the given vector
	 */
	void move( const Vector3& offset );
};


#endif