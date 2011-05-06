#ifndef _OPENCRAFTCORE_H_
#define _OPENCRAFTCORE_H_
#include "prerequisites.h"

class Renderer;
class WorldChunk;

/** @class OpencraftCore
 * Responsible for managing stuff 
 */
class OpencraftCore
{
protected:
	sf::Window	mWindow;
	sf::Clock	mClock;
	Renderer*	mRenderer;
	bool		mContinue;
	std::vector<WorldChunk*> mChunks;
public:
	OpencraftCore(void);
	~OpencraftCore(void);

	void createChunk(long x, long y, long z);

	void createWindow();

	void go();

	void exit();
};

#endif