#include "SandBlock.h"
#include "BlockFactory.h"
#include "Chunk.h"
#include "World.h"


//REGISTER_BLOCK_TYPE( "stone", StoneBlock )
GenericBlockFactory<SandBlock> sandFactory("sand");

SandBlock::SandBlock()
: BaseBlock()
{
}

SandBlock::~SandBlock(void)
{
}

void SandBlock::think( BlockContext &ctx, float dt )
{
	if( ctx.world->getBlockAt( ctx.worldX, ctx.worldY - 1, ctx.worldZ ) == NULL )
	{
		ctx.world->moveBlock( ctx.worldX, ctx.worldY, ctx.worldZ, 10.f, ctx.worldX, ctx.worldY - 1, ctx.worldZ );
	}
}

void SandBlock::connectedChange( BlockContext &ctx, short face )
{
	// If something around us changes, we may have to drop down.
	//for( long y = ctx.worldY-1; y >= 0; y-- ) {
		//BaseBlock* block = mChunk->getBlockAt(ctx.worldX, y, ctx.worldZ);
		//if( block != NULL ) {
			//if( y != ctx.worldY-1 ) {
			//	this->setPosition(ctx.worldX, y + 1, ctx.worldZ);
			//}
			//break;
		//}
	//}
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