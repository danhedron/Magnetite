#ifndef _STONEBLOCK_H_
#define _STONEBLOCK_H_
#include "prerequisites.h"
#include "BaseBlock.h"

class StoneBlock : public BaseBlock
{
protected:
	
public:
	StoneBlock(long x = 0, long y = 0, long z = 0);
	~StoneBlock(void);

	std::string getType();

	void created();
	void hit();
};

#endif