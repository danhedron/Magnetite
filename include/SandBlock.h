#ifndef _SANDBLOCK_H_
#define _SANDBLOCK_H_
#include "prerequisites.h"
#include "BaseBlock.h"

class SandBlock : public BaseBlock
{
protected:
	
public:
	SandBlock(long x = 0, long y = 0, long z = 0);
	~SandBlock(void);
	
	void connectedChange( short face );

	std::string getType();

	void created();
	void hit();
};

#endif