#ifndef _BASEGAME_H_
#define _BASEGAME_H_
#include "prerequisites.h"

class Character;
class OpencraftCore;

class BaseGame
{
protected:
	//< Reference to the engine object
	OpencraftCore* mEngine;
	//< Reference to the local player ( Or NULL if this is a dedicated server )
	Character* mPlayer;

public:
	BaseGame();
	~BaseGame();

	/**
	 * Returns the name of this Game for printing purposes.
	 */
	std::string getName();

	//<== Game Events
	/**
	 * Called when a player enters the game
	 */
	void playerJoin();

	/**
	 * Called when a player is spawning
	 */
	void playerSpawn();

	/**
	 * Called when a player is killed
	 */
	void playerKilled();

	/**
	 * Called when a character takes damage
	 */
	void characterDamage();

	/**
	 * Called When a player primary fires
	 */
	void playerPrimaryClick();

	/**
	 * Called When a player alt fires
	 */
	void playerAltClick();

};

#endif
