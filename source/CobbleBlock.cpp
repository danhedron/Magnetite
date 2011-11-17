#include "CobbleBlock.h"
#include "BlockFactory.h"

//REGISTER_BLOCK_TYPE( "stone", StoneBlock )
GenericBlockFactory<CobbleBlock> cobbleFactory("cobble");

CobbleBlock::CobbleBlock()
: BaseBlock()
{
}

CobbleBlock::~CobbleBlock(void)
{
}

void CobbleBlock::created()
{

}

void CobbleBlock::hit()
{

}

void CobbleBlock::getTextureCoords( short face, short &x, short &y )
{
	x = 2;
	y = 1;
}


std::string CobbleBlock::getType()
{
	return "cobble";
}
