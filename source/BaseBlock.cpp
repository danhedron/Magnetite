#include "BaseBlock.h"
#include "MagnetiteCore.h"
#include "TextureManager.h"
#include "Renderer.h"
#include "WorldChunk.h"
#include "World.h"
#include <string>

#include "glgeometry.h"

BaseBlock::BaseBlock()
: mDataFlags( 0 )
{
}

BaseBlock::~BaseBlock(void)
{
}

void BaseBlock::_setPosition( unsigned short x, unsigned  short y, unsigned short z)
{
	//mDataFlags = ( x | (z<<4) | (y<<8) );
}

void BaseBlock::connectedChange( short face )
{

}

/*short BaseBlock::getX() {
	return (mDataFlags & BMASK_XPOS);
}

short BaseBlock::getY() {
	return ((mDataFlags & BMASK_YPOS)>>8);
}

short BaseBlock::getZ() {
	return ((mDataFlags & BMASK_ZPOS)>>4);
}*/

void BaseBlock::updateVisFlags(unsigned int flags)
{
	mDataFlags = (mDataFlags & ~BMASK_VISFLAGS) | (mDataFlags & ~(FACE_ALL<<0));
	mDataFlags = mDataFlags | (flags<<0);
}

unsigned short BaseBlock::getVisFlags()
{
	return ((mDataFlags & BMASK_VISFLAGS)>>0);
}

void BaseBlock::getTextureCoords( short face, short &x, short &y )
{
	x = 0;
	y = 0;
}

/*void BaseBlock::setPosition( long x, long y, long z )
{
	if( ctx.chunk )
		ctx.chunk->_blockMoved( this, x, y, z );
	_setPosition(x,y,z);
}

void BaseBlock::setPosition( const Vector3& vec)
{
	setPosition( vec.x, vec.y, vec.z );
}*/

void BaseBlock::appendToStream( std::ofstream& stream )
{
	stream.write( (char *) &mDataFlags, sizeof( mDataFlags ) );	
}

void BaseBlock::readFromStream( std::ifstream& stream )
{
	stream.read( (char *) &mDataFlags, sizeof( mDataFlags ) );	
}
