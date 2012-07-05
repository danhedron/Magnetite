#ifndef _MOVINGBLOCK_H_
#define _MOVINGBLOCK_H_
#include "prerequisites.h"

class TerrainGeometry;
class BaseBlock;

struct MovingBlock
{
	BaseBlock* block;
	
	Vector3 start;
	Vector3 current;
	Vector3 end;
	
	TerrainGeometry* geom;
	
	float time;
};

typedef std::vector<MovingBlock> MovingBlockList;

#endif // _MOVINGBLOCK_H_