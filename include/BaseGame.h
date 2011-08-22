#ifndef _BASEGAME_H_
#define _BASEGAME_H_
#include "prerequisites.h"
#include "InputManager.h"

class Character;
class MagnetiteCore;

class BaseGame
{
protected:
	//< Reference to the engine object
	MagnetiteCore* mEngine;
	//< Reference to the local player ( Or NULL if this is a dedicated server )
	Character* mPlayer;
	//< Keeps track of singleplayer ness
	bool mIsSinglePlayer;

public:
	BaseGame();
	~BaseGame();

	/**
	 * Returns the name of this Game for printing purposes.
	 */
	std::string getName();

	/**
	 * Returns true in singleplayer mode 
	 */
	bool isSingleplayer();

	/**
	 * Creates a new, run of the mill character
	 */
	Character* createCharacter();

	/**
	 * Returns the local player
	 */
	Character* getLocalPlayer();

	//<== Game Events
	/**
	 * Called when a player enters the game
	 */
	void playerJoin( Character* player );

	/**
	 * Called when a player is spawning
	 */
	void playerSpawn( Character* player );

	/**
	 * Called when a player is killed
	 */
	void playerKilled( Character* player );

	/**
	 * Called when a character takes damage
	 */
	void characterDamage( Character* player );

	/**
	 * Called When a player primary fires
	 */
	void playerPrimaryClick( Character* player );

	/**
	 * Called When a player alt fires
	 */
	void playerAltClick( Character* player );

	//========================= Internal stuff

	/**
	 * Starts a singleplayer instance of this game
	 */
	void _startGameSingle();

	/**
	 * Tells the game that this is a Multiplayer game
	 */
	void _startGame();

	/**
	 * 
	 */
	void _playerJoined();

	/**
	 * Called on local input events
	 */
	void _inputEvents( const InputEvent& e );

	/**
	 * Called when a local player tries to move
	 */
	void _inputMovement( const Vector3& v );

	/**
	 * Called when a local player tries to sprint
	 */
	void _inputSprint( const Vector3& v );


	/**
	 * Called when a local player tries to look
	 */
	void _mouseMoved( const float x, const float y );

	/**
	 * Called when the local player click
	 */
	void _primary();

	/**
	 * Called when the local player clicks
	 */
	void _secondary();
};

#endif
