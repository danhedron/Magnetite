#include "GrassBlock.h"
#include "BlockFactory.h"

//REGISTER_BLOCK_TYPE( "stone", StoneBlock )
GenericBlockFactory<GrassBlock> grassFactory("grass");

GrassBlock::GrassBlock(long x, long y, long z)
: BaseBlock()
{
	texture(3,0);
}

GrassBlock::~GrassBlock(void)
{
}

void GrassBlock::created()
{

}

void GrassBlock::getTextureCoords( short face, short &x, short &y )
{
	BaseBlock::getTextureCoords( face, x, y );
	if( face == FACE_TOP )
		x = 0;
}

void GrassBlock::hit()
{

}

std::string GrassBlock::getType()
{
	return "grass";
}