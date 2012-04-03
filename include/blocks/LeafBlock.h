#ifndef _LEAFBLOCK_H_
#define _LEAFBLOCK_H_
#include "prerequisites.h"
#include "BaseBlock.h"

class LeafBlock : public BaseBlock
{
protected:
	
public:
	LeafBlock(long x = 0, long y = 0, long z = 0);
	~LeafBlock(void);

	std::string getType();

	void getTextureCoords( short face, short &x, short &y );
	
	virtual bool isOpaque();

	void created();
	void hit();
};

#endif