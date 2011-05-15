#ifndef _DIRTBLOCK_H_
#define _DIRTBLOCK_H_
#include "prerequisites.h"
#include "BaseBlock.h"

class DirtBlock : public BaseBlock
{
protected:
	
public:
	DirtBlock(long x = 0, long y = 0, long z = 0);
	~DirtBlock(void);

	std::string getType();

	void created();
	void hit();
};

#endif