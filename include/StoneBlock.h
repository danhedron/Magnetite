#ifndef _STONEBLOCK_H_
#define _STONEBLOCK_H_
#include "prerequisites.h"
#include "BaseBlock.h"

class StoneBlock : public BaseBlock
{
protected:
	
public:
	StoneBlock();
	~StoneBlock(void);

	std::string getType();

	void getTextureCoords( short face, short &x, short &y );

	void created();
	void hit();
};

#endif