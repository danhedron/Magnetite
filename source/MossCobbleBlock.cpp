#include "MossCobbleBlock.h"
#include "BlockFactory.h"

//REGISTER_BLOCK_TYPE( "stone", StoneBlock )
GenericBlockFactory<MossCobbleBlock> mossCobleFactory("mossycobble");

MossCobbleBlock::MossCobbleBlock()
: BaseBlock()
{
}

MossCobbleBlock::~MossCobbleBlock(void)
{
}

void MossCobbleBlock::created()
{

}

void MossCobbleBlock::hit()
{

}

void MossCobbleBlock::getTextureCoords( short face, short &x, short &y )
{
	x = 3;
	y = 1;
}


std::string MossCobbleBlock::getType()
{
	return "mossycobble";
}
