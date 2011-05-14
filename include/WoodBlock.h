#ifndef _WOODBLOCK_H_
#define _WOODBLOCK_H_
#include "prerequisites.h"
#include "BaseBlock.h"

class WoodBlock : public BaseBlock
{
protected:
	
public:
	WoodBlock(long x = 0, long y = 0, long z = 0);
	~WoodBlock(void);

	std::string getType();

	void created();
	void hit();
};

#endif