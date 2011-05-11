#ifndef _BASEBLOCK_H_
#define _BASEBLOCK_H_
#include "prerequisites.h"


enum {
	VIS_NONE	= 0,
	VIS_TOP		= (1<<0),
	VIS_BOTTOM	= (1<<1),
	VIS_LEFT	= (1<<2),
	VIS_RIGHT	= (1<<3),
	VIS_FORWARD	= (1<<4),
	VIS_BACK	= (1<<5)
};

class BaseBlock
{
protected:
	size_t mDamage;
	long mX;
	long mY;
	long mZ;
	short mBlockX;
	short mBlockY;
public:
	BaseBlock(long x, long y, long z);
	~BaseBlock(void);

	short mViewFlags;

	virtual void created() = 0;
	virtual void hit() = 0;

	virtual void texture(int x, int y);
	short getTextureX();
	short getTextureY();

	virtual std::string getType() = 0;

	long getX();
	long getY();
	long getZ();
};

#endif