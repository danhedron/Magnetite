#ifndef _MOSSCOBBLELOCK_H_
#define _MOSSCOBBLEBLOCK_H_
#include "prerequisites.h"
#include "BaseBlock.h"

class MossCobbleBlock : public BaseBlock
{
protected:
	
public:
	MossCobbleBlock();
	~MossCobbleBlock(void);

	std::string getType();

	void getTextureCoords( short face, short &x, short &y );

	void created();
	void hit();
};

#endif