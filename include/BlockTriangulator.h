#ifndef _BLOCKTRIANGULATOR_H_
#define _BLOCKTRIANGULATOR_H_
#include "BaseTriangulator.h"

class Chunk;

class BlockTriangulator : public BaseTriangulator
{
public:
	virtual void triangulateChunk( TerrainGeometry* geom, Chunk* chunk );
};

#endif