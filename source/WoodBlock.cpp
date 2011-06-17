#include "WoodBlock.h"
#include "BlockFactory.h"

//REGISTER_BLOCK_TYPE( "wood", WoodBlock )
GenericBlockFactory<WoodBlock> woodFactory("wood");


WoodBlock::WoodBlock()
: BaseBlock()
{
}

WoodBlock::~WoodBlock(void)
{
}

void WoodBlock::created()
{

}

void WoodBlock::hit()
{

}

void WoodBlock::getTextureCoords( short face, short &x, short &y )
{
	x = 4;
	y = 0;
}


std::string WoodBlock::getType()
{
	return "wood";
}