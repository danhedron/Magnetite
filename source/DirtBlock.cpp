#include "DirtBlock.h"
#include "BlockFactory.h"

//REGISTER_BLOCK_TYPE( "stone", StoneBlock )
GenericBlockFactory<DirtBlock> dirtFactory("dirt");

DirtBlock::DirtBlock(long x, long y, long z)
: BaseBlock()
{
	texture(2,0);
}

DirtBlock::~DirtBlock(void)
{
}

void DirtBlock::created()
{

}

void DirtBlock::hit()
{

}

std::string DirtBlock::getType()
{
	return "dirt";
}