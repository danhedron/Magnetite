#include "StoneBlock.h"
#include "BlockFactory.h"

//REGISTER_BLOCK_TYPE( "stone", StoneBlock )
GenericBlockFactory<StoneBlock> stoneFactory("stone");

StoneBlock::StoneBlock()
: BaseBlock()
{
}

StoneBlock::~StoneBlock(void)
{
}

void StoneBlock::created()
{

}

void StoneBlock::hit()
{

}

void StoneBlock::getTextureCoords( short face, short &x, short &y )
{
	x = 1;
	y = 0;
}


std::string StoneBlock::getType()
{
	return "stone";
}