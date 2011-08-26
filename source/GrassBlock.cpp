#include "GrassBlock.h"
#include "BlockFactory.h"

//REGISTER_BLOCK_TYPE( "stone", StoneBlock )
GenericBlockFactory<GrassBlock> grassFactory("grass");

GrassBlock::GrassBlock(long x, long y, long z)
: BaseBlock()
{
}

GrassBlock::~GrassBlock(void)
{
}

void GrassBlock::created()
{

}

void GrassBlock::getTextureCoords( short face, short &x, short &y )
{
	x = 1;
	if( face == FACE_TOP )
		x = 0;
	else if( face == FACE_BOTTOM )
		x = 2;
	y = 0;
}

void GrassBlock::hit()
{

}

std::string GrassBlock::getType()
{
	return "grass";
}