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

void BaseBlock::getTextureCoords( short face, short &x, short &y )
{
	x = mBlockX;
	y = mBlockY;
}

void BaseBlock::setPosition( long x, long y, long z )
{
	mX = x;
	mY = y;
	mZ = z;
}

void BaseBlock::setPosition( const Vector3& vec)
{
	setPosition( vec.x, vec.y, vec.z );
}
