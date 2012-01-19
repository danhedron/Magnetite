#ifndef _CHUNKGENERATOR_H_
#define _CHUNKGENERATOR_H_
#include "prerequisites.h"

class Chunk;
class World;

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
	float interpolatedNoise( float x, float y );
	float smooth( float x, float y );

	/**
	 * This is partical deprecated.
	 * \deprecated Use fillRegion instead
	 */
	void fillChunk( Chunk* chunk );
	
	/**
	 * Fills a region of the world with data from this generator.
	 * @param w Pointer to the world object.
	 * @param min The minimum of the bounding box to fill.
	 * @param max The maximum of the bounding box to fill.
	 */
	void fillRegion( World* w, const Vector3& min, const Vector3& max );
};


#endif // _CHUNKGENERATOR_H_