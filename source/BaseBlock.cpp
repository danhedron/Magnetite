#include "BaseBlock.h"
#include "OpencraftCore.h"
#include "TextureManager.h"

BaseBlock::BaseBlock(long x, long y, long z)
: mDamage( 0 ),
mX( x ),
mY( y ),
mZ( z ),
mViewFlags(0),
mBlockX(0),
mBlockY(0)
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

void BaseBlock::texture(int x, int y)
{
	mBlockX = x;
	mBlockY = y;
}

short BaseBlock::getTextureX()
{
	return mBlockX;
}
short BaseBlock::getTextureY()
{
	return mBlockY;
}