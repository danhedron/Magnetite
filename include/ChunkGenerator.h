#ifndef _CHUNKGENERATOR_H_
#define _CHUNKGENERATOR_H_
#include "prerequisites.h"

class WorldChunk;

class ChunkGenerator
{
protected:
	long mSeed;

public:
	ChunkGenerator( long seed );
	~ChunkGenerator();

	void setSeed( long seed );
	float noise( long x, long z );
	float interpolateCosine( float a, float b, float x );
	float smooth( float x, float y );

	void fillChunk( WorldChunk* chunk );
};


#endif // _CHUNKGENERATOR_H_