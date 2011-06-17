#include "SandBlock.h"
#include "BlockFactory.h"
#include "WorldChunk.h"

//REGISTER_BLOCK_TYPE( "stone", StoneBlock )
GenericBlockFactory<SandBlock> sandFactory("sand");

SandBlock::SandBlock()
: BaseBlock()
{
}

SandBlock::~SandBlock(void)
{
}

void SandBlock::connectedChange( short face )
{
	// If something around us changes, we may have to drop down.
	for( long y = getY()-1; y >= 0; y-- ) {
		BaseBlock* block = mChunk->getBlockAt(getX(), y, getZ());
		if( block != NULL ) {
			if( y != getY()-1 ) {
				this->setPosition(getX(), y + 1, getZ());
			}
			break;
		}
	}
}

void SandBlock::created()
{

}

void SandBlock::hit()
{

}


void SandBlock::getTextureCoords( short face, short &x, short &y )
{
	x = 2;
	y = 1;
}


std::string SandBlock::getType()
{
	return "sand";
}