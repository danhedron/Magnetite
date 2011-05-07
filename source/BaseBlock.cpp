#include "BaseBlock.h"

BaseBlock::BaseBlock(long x, long y, long z)
: mDamage( 0 ),
mX( x ),
mY( y ),
mZ( z )
{
}

BaseBlock::~BaseBlock(void)
{
}

long BaseBlock::getX() {
	return mX;
}

long BaseBlock::getY() {
	return mY;
}

long BaseBlock::getZ() {
	return mZ;
}