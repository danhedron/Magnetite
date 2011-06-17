#ifndef _SANDBLOCK_H_
#define _SANDBLOCK_H_
#include "prerequisites.h"
#include "BaseBlock.h"

class SandBlock : public BaseBlock
{
protected:
	
public:
	SandBlock();
	~SandBlock(void);
	
	void connectedChange( short face );

	std::string getType();
	void getTextureCoords( short face, short &x, short &y );

	void created();
	void hit();
};

#endif