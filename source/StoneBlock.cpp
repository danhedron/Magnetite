#include "StoneBlock.h"
#include "BlockFactory.h"

//REGISTER_BLOCK_TYPE( "stone", StoneBlock )
GenericBlockFactory<StoneBlock> stoneFactory("stone");

StoneBlock::StoneBlock(long x, long y, long z)
: BaseBlock()
{
	texture(1,0);
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

std::string StoneBlock::getType()
{
	return "stone";
}