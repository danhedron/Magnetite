#ifndef _LIGHTINGMANAGER_H_
#define _LIGHTINGMANAGER_H_
#include "prerequisites.h"

class WorldChunk;

class LightingManager
{
protected:
	size_t litChunks;	
public:
	LightingManager();
	~LightingManager();

	static void lightChunk( WorldChunk* chnk );
	static void lightColumn( WorldChunk* chunk, size_t x, size_t z );
};

#endif
