#ifndef _MOVINGBLOCK_H_
#define _MOVINGBLOCK_H_
#include "prerequisites.h"

class BaseBlock;
class GLgeometry;

struct MovingBlock
{
	BaseBlock* block;
	
	Vector3 start;
	Vector3 current;
	Vector3 end;
	
	GLgeometry* geom;
	
	float time;
};

typedef std::vector<MovingBlock> MovingBlockList;

#endif // _MOVINGBLOCK_H_