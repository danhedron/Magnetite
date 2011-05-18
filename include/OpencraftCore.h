#ifndef _OPENCRAFTCORE_H_
#define _OPENCRAFTCORE_H_
#include "prerequisites.h"

class Renderer;
class TextureManager;
class World;
class Character;

/** @class OpencraftCore
 * Responsible for managing stuff 
 */
class OpencraftCore
{
protected:
	sf::Window	mWindow;
	sf::Clock	mClock;
	Renderer*	mRenderer;
	TextureManager* mTextureManager;
	World*		mWorld;
	bool		mContinue;
	Character*	mPlayer;
	float		mTimescale;
public:
	OpencraftCore(void);
	~OpencraftCore(void);

	/**
	 * Singleton instance of the OpencraftCore
	 */
	static OpencraftCore *Singleton;

	/**
	 * @return a pointer to the Texture Manager
	 */
	TextureManager *getTextureManager();

	/**
	 * Removes the block the player is looking at.
	 */
	void removeEyeBlock();

	/**
	 * Places a block where the player is looking.
	 */
	void placeEyeBlock();

	/**
	 * Creates a new world
	 */
	void newWorld( std::string name );

	/**
	 * loads an exsisting world.
	*/
	void loadWorld( std::string name );

	/**
	 * Retuns a pointer to the world.
	 */
	World* getWorld();

	/**
	 * Unloads the currently loaded world (if any)
	 */
	void unloadWorld();

	/**
	 * Creates a character
	 */
	Character* createCharacter();

	/** 
	 * Returns the timescale
	 */
	float getTimescale();

	
	void createWindow(int *argc, char **argv);

	void go(int *argc, char **argv);

	void exit();
};

#endif