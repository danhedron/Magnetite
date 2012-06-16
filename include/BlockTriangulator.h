#ifndef _BLOCKTRIANGULATOR_H_
#define _BLOCKTRIANGULATOR_H_
#include "BaseTriangulator.h"

class Chunk;
class GLgeometry;
class BlockTriangulator : public BaseTriangulator
{
public:
	virtual void triangulateChunk( GLgeometry* geom, Chunk* chunk );
};

#endif