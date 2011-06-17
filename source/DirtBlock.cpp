#include "DirtBlock.h"
#include "BlockFactory.h"

//REGISTER_BLOCK_TYPE( "stone", StoneBlock )
GenericBlockFactory<DirtBlock> dirtFactory("dirt");

DirtBlock::DirtBlock(long x, long y, long z)
: BaseBlock()
{
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


void DirtBlock::getTextureCoords( short face, short &x, short &y )
{
	x = 2;
	y = 0;
}


std::string DirtBlock::getType()
{
	return "dirt";
}