#include "WoodBlock.h"

WoodBlock::WoodBlock(long x, long y, long z)
: BaseBlock( x, y, z)
{
	texture(4,0);
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

std::string WoodBlock::getType()
{
	return "wood";
}