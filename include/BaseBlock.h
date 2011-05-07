#ifndef _BASEBLOCK_H_
#define _BASEBLOCK_H_
#include "prerequisites.h"

class BaseBlock
{
protected:
	size_t mDamage;
	long mX;
	long mY;
	long mZ;
public:
	BaseBlock(long x, long y, long z);
	~BaseBlock(void);

	virtual void created() = 0;
	virtual void hit() = 0;

	long getX();
	long getY();
	long getZ();
};

#endif