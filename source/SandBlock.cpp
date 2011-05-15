#include "SandBlock.h"
#include "BlockFactory.h"
#include "WorldChunk.h"

//REGISTER_BLOCK_TYPE( "stone", StoneBlock )
GenericBlockFactory<SandBlock> sandFactory("sand");

SandBlock::SandBlock(long x, long y, long z)
: BaseBlock()
{
	texture(2,1);
}

SandBlock::~SandBlock(void)
{
}

void SandBlock::connectedChange( short face )
{
	// If something around us changes, we may have to drop down.
	for( long y = mY-1; y >= 0; y-- ) {
		BaseBlock* block = mChunk->getBlockAt(mX, y, mZ);
		if( block != NULL ) {
			if( y != mY-1 ) {
				this->setPosition(mX, y + 1, mZ);
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

std::string SandBlock::getType()
{
	return "sand";
}