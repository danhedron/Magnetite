#ifndef _COBBLELOCK_H_
#define _COBBLEBLOCK_H_
#include "prerequisites.h"
#include "BaseBlock.h"

class CobbleBlock : public BaseBlock
{
protected:
	
public:
	CobbleBlock();
	~CobbleBlock(void);

	std::string getType();

	void getTextureCoords( short face, short &x, short &y );

	void created();
	void hit();
};

#endif