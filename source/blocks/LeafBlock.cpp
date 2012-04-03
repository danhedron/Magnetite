#include "LeafBlock.h"
#include "BlockFactory.h"

//REGISTER_BLOCK_TYPE( "stone", StoneBlock )
GenericBlockFactory<LeafBlock> leafFactory("leaf");

LeafBlock::LeafBlock(long x, long y, long z)
: BaseBlock()
{
}

LeafBlock::~LeafBlock(void)
{
}

void LeafBlock::created()
{

}

void LeafBlock::hit()
{

}

void LeafBlock::getTextureCoords( short face, short &x, short &y )
{
	x = 2;
	y = 2;
}

std::string LeafBlock::getType()
{
	return "leaf";
}