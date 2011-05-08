#include "StoneBlock.h"

StoneBlock::StoneBlock(long x, long y, long z)
: BaseBlock( x, y, z)
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