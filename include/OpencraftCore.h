#ifndef _OPENCRAFTCORE_H_
#define _OPENCRAFTCORE_H_
#include "prerequisites.h"

class Renderer;

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
public:
	OpencraftCore(void);
	~OpencraftCore(void);

	void createWindow();

	void go();

	void exit();
};

#endif