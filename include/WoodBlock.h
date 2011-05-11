#ifndef _WOODBLOCK_H_
#define _WOODBLOCK_H_
#include "prerequisites.h"
#include "BaseBlock.h"

class WoodBlock : public BaseBlock
{
protected:
	
public:
	WoodBlock(long x, long y, long z);
	~WoodBlock(void);

	std::string getType();

	void created();
	void hit();
};

#endif