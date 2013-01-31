#ifndef _BASEGAME_H_
#define _BASEGAME_H_
#include "prerequisites.h"
#include "InputManager.h"

class Player;
class Character;
class MagnetiteCore;
class Renderer;

class BaseGame
{
protected:
	/**
	 * Core pointer (so that we can pretend it's not a singleton).
	 */
	MagnetiteCore* mEngine;
	
	/**
	 * Refrence to the local player, if applicable.
	 */
	Player* mPlayer;
	
	/**
	 * True if the game is running without multiplayer.
	 */
	bool mIsSinglePlayer;
	
public:
	BaseGame();
	~BaseGame();

	/**
	 * Returns the name of this Game for printing purposes.
	 */
	virtual std::string getName();
	
	/**
	 * Returns the path for this game, used for scripted games
	 */
	virtual Magnetite::String getPath();

	/**
	 * Returns true in singleplayer mode 
	 */
	bool isSingleplayer();

	/**
	 * Returns the local player
	 */
	Player* getLocalPlayer();

	//<== Game Events
	/**
	 * Called when a player enters the game
	 */
	virtual void playerJoin( Player* player );

	/**
	 * Called when a player is spawning
	 */
	virtual void playerSpawn( Player* player );

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
	virtual void playerPrimaryClick( Player* player );

	/**
	 * Called When a player alt fires
	 */
	virtual void playerAltClick( Player* player );
	
	/**
	 * Called when the game is ok to render the GUI
	 */
	virtual void uiPaint(Renderer* r);
	
	/**
	 * Called when the engine thinks
	 */
	virtual void think( float dt );
	
	/**
	 * Generic key down event
	 */
	virtual void keyDown( size_t evt );
	
	/**
	 * Generic key up event
	 */
	virtual void keyUp( size_t evt );

	//========================= Internal stuff
	/**
	 * Starts a singleplayer instance of this game
	 */
	virtual void _startGameSingle();

	/**
	 * Tells the game that this is a Multiplayer game
	 */
	virtual void _startGame();
	
	/**
	 * Called when the world has just been loaded
	 */
	virtual void _loadGame();

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
	virtual void _mouseMoved( const float x, const float y );

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
