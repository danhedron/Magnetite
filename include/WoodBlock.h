#ifndef _WOODBLOCK_H_
#define _WOODBLOCK_H_
#include "prerequisites.h"
#include "BaseBlock.h"

class WoodBlock : public BaseBlock
{
protected:
	
public:
	WoodBlock();
	~WoodBlock(void);

	std::string getType();

	void getTextureCoords( short face, short &x, short &y );

	void created();
	void hit();
};

#endif