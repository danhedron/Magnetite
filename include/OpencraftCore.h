#ifndef _OPENCRAFTCORE_H_
#define _OPENCRAFTCORE_H_
#include "prerequisites.h"

class Renderer;
class WorldChunk;
class TextureManager;

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
	bool		mContinue;
	ChunkList	mChunks;
	int			radius;
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
	 * Creates a chunk at the given coordinates and fills it with test data
	 * @param x Coordinate
	 * @param y Coordinate
	 * @param z Coordinate
	 */
	void createChunk(long x, long y, long z);

	void removeChunk(long x, long y, long z);

	void createTestChunks( int size );
	void destoryWorld();

	WorldChunk* getChunk(const long x, const long y, const long z);

	void createWindow(int *argc, char **argv);

	void go(int *argc, char **argv);

	void exit();
};

#endif