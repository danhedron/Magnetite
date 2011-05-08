#include "WoodBlock.h"

WoodBlock::WoodBlock(long x, long y, long z)
: BaseBlock( x, y, z)
{
	texture(0,1);
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