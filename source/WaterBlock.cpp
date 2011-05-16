#include "WaterBlock.h"
#include "BlockFactory.h"
#include "WorldChunk.h"
#include "TextureManager.h"
#include "OpencraftCore.h"
#include "Renderer.h"

//REGISTER_BLOCK_TYPE( "stone", StoneBlock )
GenericBlockFactory<WaterBlock> waterFactory("water");

WaterBlock::WaterBlock(long x, long y, long z)
: BaseBlock(),
mFluidLevel( 100.f ),
mIsNew( true )
{
	texture(13,12);
}

WaterBlock::~WaterBlock(void)
{
}

void WaterBlock::created()
{

}

bool WaterBlock::isFluid()
{
	return true;
}

bool WaterBlock::isOpaque()
{
	return false;
}

void WaterBlock::getTextureCoords( short face, short &x, short &y )
{
	BaseBlock::getTextureCoords( face, x, y );
}

void WaterBlock::connectedChange( short face )
{

}

float WaterBlock::getFluidLevel()
{
	return mFluidLevel;
}

void WaterBlock::changeFluidLevel( float delta )
{
	setFluidLevel(mFluidLevel + delta);
}

void WaterBlock::setFluidLevel( float delta )
{
	mFluidLevel = delta;
	if( mChunk ) {
		mChunk->markModified();
		if( mFluidLevel <= 1.0f )
			mChunk->_addBlockToRemoveList(this);
	}
}

static float FLOW_MAX = 1.0f; // Maximum flow per second

void WaterBlock::balanceFluid( BaseBlock* block, float dt )
{
	if( block->getType() == this->getType() )
	{
		WaterBlock* water = (WaterBlock*)block;
		float dif = mFluidLevel - water->getFluidLevel();
		if( dif > 0.0001f ) {
			//dif = std::min(dif, FLOW_MAX * dt);
			water->changeFluidLevel( dif / 2.f );
			changeFluidLevel( -dif / 2.f );
		}
	}
}

void WaterBlock::flow( float dt )
{
	if( mIsNew == true ) {
		mIsNew = false;
		return;
	}
	
	if( mFluidLevel < 10.f ) {
		//Less than 10% will slowly evaporate.
		changeFluidLevel( -0.5f * dt );
	}

	// Less than 2% doesn't spread
	if( mFluidLevel < 1.f )
		return;


	BaseBlock* t = NULL;
	/* Check -Z */
	t = mChunk->getBlockAt( mX, mY, mZ - 1 );
	if( t != NULL && t->isFluid() ) {
		balanceFluid( t, dt );
	}
	else if( t == NULL && mZ - 1 >= 0 ) {
		WaterBlock* block = new WaterBlock();
		block->setPosition( mX, mY, mZ - 1 );
		changeFluidLevel( -mFluidLevel / 2.f );
		block->setFluidLevel( mFluidLevel );
		mChunk->addBlockToChunk( block );
	}
	/* Check +Z */
	t = mChunk->getBlockAt( mX, mY, mZ + 1 );
	if( t != NULL && t->isFluid() ) {
		balanceFluid( t, dt );
	}
	else if( t == NULL && mZ + 1 < CHUNK_WIDTH )  {
		WaterBlock* block = new WaterBlock();
		block->setPosition( mX, mY, mZ + 1 );
		changeFluidLevel( -mFluidLevel / 2.f );
		block->setFluidLevel( mFluidLevel );
		mChunk->addBlockToChunk( block );
	}
	/* Check +X */
	t = mChunk->getBlockAt( mX + 1, mY, mZ );
	if( t != NULL && t->isFluid() ) {
		balanceFluid( t, dt );
	}
	else if( t == NULL && mX + 1 < CHUNK_WIDTH )  {
		WaterBlock* block = new WaterBlock();
		block->setPosition( mX + 1, mY, mZ );
		changeFluidLevel( -mFluidLevel / 2.f );
		block->setFluidLevel( mFluidLevel );
		mChunk->addBlockToChunk( block );
	}
	/* Check -X */
	t = mChunk->getBlockAt( mX - 1, mY, mZ );
	if( t != NULL && t->isFluid() ) {
		balanceFluid( t, dt );
	}
	else if( t == NULL && mX - 1 >= 0 )  {
		WaterBlock* block = new WaterBlock();
		block->setPosition( mX - 1, mY, mZ );
		changeFluidLevel( -mFluidLevel / 2.f );
		block->setFluidLevel( mFluidLevel );
		mChunk->addBlockToChunk( block );
	}
}

void WaterBlock::hit()
{

}

std::string WaterBlock::getType()
{
	return "water";
}

void WaterBlock::buildCubeData(size_t& ind, size_t& eInd, GLvertex* data, GLedge* edges)
{
	short x = 0, y = 0;
	this->vertexIndex = ind;

	// Calculate the UVs based on visibility.
	/*float y = 0.f;
	for( size_t f = 0; f < 6; f++ ) {
		if( (block->mViewFlags & (1<<f)) == (1<<f) ) 
			y += 0.0625f;
	}

	rect.x = 0;
	rect.w = 0.0625f;
	rect.y = y;
	rect.h = 0.0625f;*/

	float fluidHeight = mFluidLevel / 100.f;

	/* Face -Z */
	if((this->mViewFlags & FACE_BACK) == FACE_BACK ) {
		this->getTextureCoords( FACE_BACK, x, y );
		GLuvrect rect = OpencraftCore::Singleton->getTextureManager()->getBlockUVs( x, y );

		data[ind + 0] = Renderer::vertex( this->getX() + 1.0f, this->getY() + fluidHeight,	this->getZ() + 1.0f, // Coordinates
								0.0f, 0.0f, -1.0f,
								rect.x, rect.y );
		data[ind + 1] = Renderer::vertex( this->getX() - 0.0f, this->getY() + fluidHeight, this->getZ() + 1.0f, // Coordinates
								0.0f, 0.0f, -1.0f,
								rect.x + rect.w, rect.y );
		data[ind + 2] = Renderer::vertex( this->getX() - 0.0f, this->getY() - 0.0f, this->getZ() + 1.0f, // Coordinates
								0.0f, 0.0f, -1.0f,
								rect.x + rect.w, rect.y + rect.h );
		data[ind + 3] = Renderer::vertex( this->getX() + 1.0f, this->getY() - 0.0f, this->getZ() + 1.0f, // Coordinates
								0.0f, 0.0f, -1.0f,
								rect.x, rect.y + rect.h );
		edges[eInd + 0] = ind + 2; edges[eInd + 1] = ind + 1; edges[eInd + 2] = ind + 0;
		edges[eInd + 3] = ind + 2; edges[eInd + 4] = ind + 0; edges[eInd + 5] = ind + 3;
		eInd += 6;
 		ind += 4;
	}
	/* Face +Z */
	if((this->mViewFlags & FACE_FORWARD) == FACE_FORWARD ) {
		this->getTextureCoords( FACE_FORWARD, x, y );
		GLuvrect rect = OpencraftCore::Singleton->getTextureManager()->getBlockUVs( x, y );

		data[ind + 0] = Renderer::vertex( this->getX() + 1.0f, this->getY() + fluidHeight, this->getZ() - 0.0f, // Coordinates
								0.0f, 0.0f, 1.0f,
								rect.x, rect.y );
		data[ind + 1] = Renderer::vertex( this->getX() - 0.0f, this->getY() + fluidHeight, this->getZ() - 0.0f, // Coordinates
								0.0f, 0.0f, 1.0f,
								rect.x + rect.w, rect.y );
		data[ind + 2] = Renderer::vertex( this->getX() - 0.0f, this->getY() - 0.0f, this->getZ() - 0.0f, // Coordinates
								0.0f, 0.0f, 1.0f,
								rect.x + rect.w, rect.y + rect.h );
		data[ind + 3] = Renderer::vertex( this->getX() + 1.0f, this->getY() - 0.0f, this->getZ() - 0.0f, // Coordinates
								0.0f, 0.0f, 1.0f,
								rect.x, rect.y + rect.h );
		edges[eInd + 5] = ind + 2; edges[eInd + 4] = ind + 1; edges[eInd + 3] = ind + 0;
		edges[eInd + 2] = ind + 2; edges[eInd + 1] = ind + 0; edges[eInd + 0] = ind + 3;
		eInd += 6;
		ind += 4;
	}
	/* Face +X */
	if((this->mViewFlags & FACE_RIGHT) == FACE_RIGHT) {
		this->getTextureCoords( FACE_RIGHT, x, y );
		GLuvrect rect = OpencraftCore::Singleton->getTextureManager()->getBlockUVs( x, y );

		data[ind + 0] = Renderer::vertex( this->getX() + 1.0f, this->getY() + fluidHeight, this->getZ() + 1.0f, // Coordinates
								1.0f, 0.0f, 0.0f,
								rect.x + rect.w, rect.y );
		data[ind + 1] = Renderer::vertex( this->getX() + 1.0f, this->getY() - 0.0f, this->getZ() + 1.0f, // Coordinates
								1.0f, 0.0f, 0.0f,
								rect.x + rect.w, rect.y + rect.h );
		data[ind + 2] = Renderer::vertex( this->getX() + 1.0f, this->getY() - 0.0f, this->getZ() - 0.0f, // Coordinates
								1.0f, 0.0f, 0.0f,
								rect.x, rect.y + rect.h );
		data[ind + 3] = Renderer::vertex( this->getX() + 1.0f, this->getY() + fluidHeight, this->getZ() - 0.0f, // Coordinates
								1.0f, 0.0f, 0.0f,
								rect.x, rect.y );
		edges[eInd + 0] = ind + 2; edges[eInd + 1] = ind + 1; edges[eInd + 2] = ind + 0;
		edges[eInd + 3] = ind + 2; edges[eInd + 4] = ind + 0; edges[eInd + 5] = ind + 3;
		eInd += 6;
		ind += 4;
	}
	/* Face -Y */
	if((this->mViewFlags & FACE_BOTTOM) == FACE_BOTTOM) {
		this->getTextureCoords( FACE_BOTTOM, x, y );
		GLuvrect rect = OpencraftCore::Singleton->getTextureManager()->getBlockUVs( x, y );

		data[ind + 0] = Renderer::vertex( this->getX() - 0.0f, this->getY() - 0.0f, this->getZ() + 1.0f, // Coordinates
								0.0f, -1.0f, 0.0f,
								rect.x, rect.y );
		data[ind + 1] = Renderer::vertex( this->getX() - 0.0f, this->getY() - 0.0f, this->getZ() - 0.0f, // Coordinates
								0.0f, -1.0f, 0.0f,
								rect.x + rect.w, rect.y );
		data[ind + 2] = Renderer::vertex( this->getX() + 1.0f, this->getY() - 0.0f, this->getZ() - 0.0f, // Coordinates
								0.0f, -1.0f, 0.0f,
								rect.x + rect.w, rect.y + rect.h );
		data[ind + 3] = Renderer::vertex( this->getX() + 1.0f, this->getY() - 0.0f, this->getZ() + 1.0f, // Coordinates
								0.0f, -1.0f, 0.0f,
								rect.x, rect.y + rect.h );
		edges[eInd + 0] = ind + 2; edges[eInd + 1] = ind + 1; edges[eInd + 2] = ind + 0;
		edges[eInd + 3] = ind + 2; edges[eInd + 4] = ind + 0; edges[eInd + 5] = ind + 3;
		eInd += 6;
		ind += 4;
	}
	/* Face +Y */
	if((this->mViewFlags & FACE_TOP) == FACE_TOP) {
		this->getTextureCoords( FACE_TOP, x, y );
		GLuvrect rect = OpencraftCore::Singleton->getTextureManager()->getBlockUVs( x, y );

		data[ind + 0] = Renderer::vertex( this->getX() - 0.0f, this->getY() + fluidHeight, this->getZ() + 1.0f, // Coordinates
								0.0f, 1.0f, 0.0f,
								rect.x, rect.y );
		data[ind + 1] = Renderer::vertex( this->getX() + 1.0f, this->getY() + fluidHeight, this->getZ() + 1.0f, // Coordinates
								0.0f, 1.0f, 0.0f,
								rect.x + rect.w, rect.y );
		data[ind + 2] = Renderer::vertex( this->getX() + 1.0f, this->getY() + fluidHeight, this->getZ() - 0.0f, // Coordinates
								0.0f, 1.0f, 0.0f,
								rect.x + rect.w, rect.y + rect.h );
		data[ind + 3] = Renderer::vertex( this->getX() - 0.0f, this->getY() + fluidHeight, this->getZ() - 0.0f, // Coordinates
								0.0f, 1.0f, 0.0f,
								rect.x, rect.y + rect.h );
		edges[eInd + 0] = ind + 2; edges[eInd + 1] = ind + 1; edges[eInd + 2] = ind + 0;
		edges[eInd + 3] = ind + 2; edges[eInd + 4] = ind + 0; edges[eInd + 5] = ind + 3;
		eInd += 6;
		ind += 4;
	}
	/* Face -X */
	if((this->mViewFlags & FACE_LEFT) == FACE_LEFT) {
		this->getTextureCoords( FACE_LEFT, x, y );
		GLuvrect rect = OpencraftCore::Singleton->getTextureManager()->getBlockUVs( x, y );

		data[ind + 0] = Renderer::vertex( this->getX() - 0.0f, this->getY() + fluidHeight, this->getZ() - 0.0f, // Coordinates
								-1.0f, 0.0f, 0.0f,
								rect.x + rect.w, rect.y );
		data[ind + 1] = Renderer::vertex( this->getX() - 0.0f, this->getY() - 0.0f, this->getZ() - 0.0f, // Coordinates
								-1.0f, 0.0f, 0.0f,
								rect.x + rect.w, rect.y + rect.h );
		data[ind + 2] = Renderer::vertex( this->getX() - 0.0f, this->getY() - 0.0f, this->getZ() + 1.0f, // Coordinates
								-1.0f, 0.0f, 0.0f,
								rect.x, rect.y + rect.h );
		data[ind + 3] = Renderer::vertex( this->getX() - 0.0f, this->getY() + fluidHeight, this->getZ() + 1.0f, // Coordinates
								-1.0f, 0.0f, 0.0f,
								rect.x, rect.y );
		edges[eInd + 0] = ind + 2; edges[eInd + 1] = ind + 1; edges[eInd + 2] = ind + 0;
		edges[eInd + 3] = ind + 2; edges[eInd + 4] = ind + 0; edges[eInd + 5] = ind + 3;
		eInd += 6;
		ind += 4;
	}
}