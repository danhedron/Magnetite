#ifndef _OPENCRAFTCORE_H_
#define _OPENCRAFTCORE_H_
#include "prerequisites.h"

class Renderer;
class TextureManager;
class World;

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
<<<<<<< HEAD
	 * loads an exsisting world.
=======
	 *	Converts world coordinates into chunk indexes.
	 */
	Vector3 worldToChunks( const Vector3& vec );

	/**
	 *	Converts world coordinates into block offset ( in the chunk at that location )
	 */
	Vector3 worldToBlock( const Vector3& vec );

	/**
	 * Performs a raycast test against a single cube
	 * @param ray Raycast object.
	 * @param min Cube minimum.
	 * @param max Cube maximum.
>>>>>>> 6a80f6ece13cafcc4c01729d8bd9731591ff5001
	 */
	void loadWorld( std::string name );

	/**
	 * Unloads the currently loaded world (if any)
	 */
	void unloadWorld();

	
	void createWindow(int *argc, char **argv);

	void go(int *argc, char **argv);

	void exit();
};

#endif