#ifndef _STONEBLOCK_H_
#define _STONEBLOCK_H_
#include "prerequisites.h"
#include "BaseBlock.h"

class StoneBlock : public BaseBlock
{
protected:
	
public:
	StoneBlock(long x, long y, long z);
	~StoneBlock(void);

	void created();
	void hit();
};

#endif