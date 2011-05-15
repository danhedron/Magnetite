#include "BaseBlock.h"
#include "OpencraftCore.h"
#include "TextureManager.h"
#include "WorldChunk.h"

BaseBlock::BaseBlock(long x, long y, long z)
: mDamage( 0 ),
mX( x ),
mY( y ),
mZ( z ),
mViewFlags(0),
mBlockX(0),
mBlockY(0),
mChunk( NULL )
{
	
}

BaseBlock::~BaseBlock(void)
{
}

void BaseBlock::connectedChange( short face )
{

}

void BaseBlock::_setChunk( WorldChunk* chnk )
{
	mChunk = chnk;
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
	if( mChunk )
		mChunk->_blockMoved( this, x, y, z );
	mX = x;
	mY = y;
	mZ = z;
}

void BaseBlock::setPosition( const Vector3& vec)
{
	setPosition( vec.x, vec.y, vec.z );
}
