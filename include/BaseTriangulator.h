#ifndef _BASETRIANGULATOR_H_
#define _BASETRIANGULATOR_H_

class Chunk;
class GLgeometry;
class BaseTriangulator
{
public:
	virtual void triangulateChunk( GLgeometry* geom, Chunk* chunk ) = 0;
};

#endif