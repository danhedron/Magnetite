#ifndef _GRASSBLOCK_H_
#define _GRASSBLOCK_H_
#include "prerequisites.h"
#include "BaseBlock.h"

class GrassBlock : public BaseBlock
{
protected:
	
public:
	GrassBlock(long x = 0, long y = 0, long z = 0);
	~GrassBlock(void);

	virtual std::string getType();

	void getTextureCoords( short face, short &x, short &y );

	void created();
	void hit();
};

#endif