#ifndef _LIGHTINGMANAGER_H_
#define _LIGHTINGMANAGER_H_
#include "prerequisites.h"

class Chunk;

class LightingManager
{
protected:
	size_t litChunks;
public:
	LightingManager();
	~LightingManager();

	enum {
		POS_X = 1,
		POS_Z = 2,
		NEG_X = 4,
		NEG_Z = 8
	};

	static void lightChunk( Chunk* chnk );
	static void lightColumn( Chunk* chunk, size_t x, size_t z, LightIndex light );
	static void lightUnderground( Chunk* chunk, size_t x, size_t startY, size_t z, size_t dir, LightIndex light );

	static void gatherLight( Chunk* chunk );
};

#endif
