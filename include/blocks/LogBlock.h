#ifndef _LOGBLOCK_H_
#define _LOGBLOCK_H_
#include "prerequisites.h"
#include "BaseBlock.h"

class LogBlock : public BaseBlock
{
protected:
	
public:
	LogBlock(long x = 0, long y = 0, long z = 0);
	~LogBlock(void);

	std::string getType();

	void getTextureCoords( short face, short &x, short &y );

	void created();
	void hit();
};

#endif