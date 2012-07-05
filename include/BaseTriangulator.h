#ifndef _BASETRIANGULATOR_H_
#define _BASETRIANGULATOR_H_
#include "prerequisites.h"

class TerrainGeometry;
class Chunk;

class BaseTriangulator
{
public:
	virtual void triangulateChunk( TerrainGeometry* geom, Chunk* chunk ) = 0;
};

#endif