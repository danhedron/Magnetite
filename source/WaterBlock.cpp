#include "WaterBlock.h"
#include "BlockFactory.h"
#include "TextureManager.h"
#include "MagnetiteCore.h"
#include "Renderer.h"

//REGISTER_BLOCK_TYPE( "stone", StoneBlock )
GenericBlockFactory<WaterBlock> waterFactory("water");

WaterBlock::WaterBlock()
: BaseBlock(),
mFluidLevel( 100.f ),
mIsNew( true )
{
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

bool WaterBlock::isSolid()
{
	return false;
}

bool WaterBlock::isThinking()
{
	return true;
}


void WaterBlock::getTextureCoords( short face, short &x, short &y )
{
	x = 13;
	y = 12;
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
	//if( mChunk ) {
	//	mChunk->markModified();
	//	if( mFluidLevel <= 1.0f )
	//		mChunk->_addBlockToRemoveList(this);
	//}
}

static float FLOW_MAX = 60.0f; // Maximum flow per second

void WaterBlock::balanceFluid( BaseBlock* block, float dt )
{
	if( block->getType() == this->getType() )
	{
		WaterBlock* water = (WaterBlock*)block;
		float dif = mFluidLevel - water->getFluidLevel();
		if( dif > 0.0001f ) {
			dif = std::min(dif, FLOW_MAX * dt);
			water->changeFluidLevel( dif / 2.f );
			changeFluidLevel( -dif / 2.f );
		}
	}
}

void WaterBlock::flowToBlock(unsigned short x, unsigned short y, unsigned short z, float dt)
{
	/*BaseBlock* t = mChunk->getBlockAt( x, y, z );
	//Check -Z
	if( t != NULL && t->isFluid() ) {
		balanceFluid( t, dt );
	}
	else if( t != NULL && !t->isFluid() ) {
	}
	else if( t == NULL ) { //&& x >= 0 && x < CHUNK_WIDTH && y >= 0 && y < CHUNK_HEIGHT && z >= 0 && z < CHUNK_WIDTH ) {
		BaseBlock* lower = mChunk->getBlockAt( x, y - 1, z );
		if( y > 0 && lower == NULL )
			y--;
		else if( y > 0 && lower->getType() == this->getType() ) {
			WaterBlock* water = (WaterBlock*)lower;
			float total = water->getFluidLevel() + mFluidLevel / 2.f;
			if( total < 100.0f ) {
				// If the lower block can accept a whole half, send it and exit
				water->changeFluidLevel( mFluidLevel / 2.f );
				changeFluidLevel( -mFluidLevel / 2.f );
				return;
			}
			else {
				float over = ((mFluidLevel / 2.f) + water->getFluidLevel()) - 100.f;
				water->setFluidLevel( 100.f );
				WaterBlock* block = new WaterBlock();
				block->setPosition(x,y,z);
				changeFluidLevel( -mFluidLevel / 2.f );
				block->setFluidLevel( over );
				mChunk->addBlockToChunk( block );
				return;
			}
		}

		WaterBlock* block = new WaterBlock();
		block->setPosition(x,y,z);
		float dif = mFluidLevel;
		dif = std::min(dif, FLOW_MAX * dt);
		changeFluidLevel( -dif / 2 );
		block->setFluidLevel( dif / 2 );
		mChunk->addBlockToChunk( block );
	}*/

}

void WaterBlock::flow( float dt )
{
	/*if( mIsNew == true ) {
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

	// If there is nothing beneath us, fall
	BaseBlock* t = mChunk->getBlockAt( getX(), getY() - 1, getZ() );
	if( t == NULL && getY() > 0 ) {
		this->setPosition( getX(), getY() - 1, getZ() );
	}
	else if( t->getType() == this->getType() )
	{
		WaterBlock* water = (WaterBlock*)t;
		float total = water->getFluidLevel() + mFluidLevel;
		if( total < 100.0f ) {
			// If the lower block can accept this whole block, just drop us and delete.
			water->changeFluidLevel( mFluidLevel );
			mChunk->_addBlockToRemoveList( this );
			return;
		}
		else {
			float over = ((mFluidLevel) + water->getFluidLevel()) - 100.f;
			water->setFluidLevel( 100.f );
			setFluidLevel( over );
		}
	}

	flowToBlock( getX(), getY(), getZ() - 1, dt);
	flowToBlock( getX(), getY(), getZ() + 1, dt );
	flowToBlock( getX() + 1, getY(), getZ(), dt );
	flowToBlock( getX() - 1, getY(), getZ(), dt );*/
}

void WaterBlock::hit()
{

}

std::string WaterBlock::getType()
{
	return "water";
}

//void WaterBlock::buildCubeData(size_t& ind, size_t& eInd, GLvertex* data, GLedge* edges)
//{
//	short x = 0, y = 0;
//
//	short visFlags = getVisFlags();
//
//	float fluidHeight = mFluidLevel / 100.f;
//
//	/* Face -Z */
//	if((visFlags & FACE_BACK) == FACE_BACK ) {
//		this->getTextureCoords( FACE_BACK, x, y );
//		GLuvrect rect = MagnetiteCore::Singleton->getTextureManager()->getBlockUVs( x, y );
//
//		data[ind + 0] = Renderer::vertex( this->getX() + 1.0f, this->getY() + fluidHeight,	this->getZ() + 1.0f, // Coordinates
//								0.0f, 0.0f, -1.0f,
//								rect.x, rect.y );
//		data[ind + 1] = Renderer::vertex( this->getX() - 0.0f, this->getY() + fluidHeight, this->getZ() + 1.0f, // Coordinates
//								0.0f, 0.0f, -1.0f,
//								rect.x + rect.w, rect.y );
//		data[ind + 2] = Renderer::vertex( this->getX() - 0.0f, this->getY() - 0.0f, this->getZ() + 1.0f, // Coordinates
//								0.0f, 0.0f, -1.0f,
//								rect.x + rect.w, rect.y + rect.h );
//		data[ind + 3] = Renderer::vertex( this->getX() + 1.0f, this->getY() - 0.0f, this->getZ() + 1.0f, // Coordinates
//								0.0f, 0.0f, -1.0f,
//								rect.x, rect.y + rect.h );
//		edges[eInd + 0] = ind + 2; edges[eInd + 1] = ind + 1; edges[eInd + 2] = ind + 0;
//		edges[eInd + 3] = ind + 2; edges[eInd + 4] = ind + 0; edges[eInd + 5] = ind + 3;
//		eInd += 6;
// 		ind += 4;
//	}
//	/* Face +Z */
//	if((visFlags & FACE_FORWARD) == FACE_FORWARD ) {
//		this->getTextureCoords( FACE_FORWARD, x, y );
//		GLuvrect rect = MagnetiteCore::Singleton->getTextureManager()->getBlockUVs( x, y );
//
//		data[ind + 0] = Renderer::vertex( this->getX() + 1.0f, this->getY() + fluidHeight, this->getZ() - 0.0f, // Coordinates
//								0.0f, 0.0f, 1.0f,
//								rect.x, rect.y );
//		data[ind + 1] = Renderer::vertex( this->getX() - 0.0f, this->getY() + fluidHeight, this->getZ() - 0.0f, // Coordinates
//								0.0f, 0.0f, 1.0f,
//								rect.x + rect.w, rect.y );
//		data[ind + 2] = Renderer::vertex( this->getX() - 0.0f, this->getY() - 0.0f, this->getZ() - 0.0f, // Coordinates
//								0.0f, 0.0f, 1.0f,
//								rect.x + rect.w, rect.y + rect.h );
//		data[ind + 3] = Renderer::vertex( this->getX() + 1.0f, this->getY() - 0.0f, this->getZ() - 0.0f, // Coordinates
//								0.0f, 0.0f, 1.0f,
//								rect.x, rect.y + rect.h );
//		edges[eInd + 5] = ind + 2; edges[eInd + 4] = ind + 1; edges[eInd + 3] = ind + 0;
//		edges[eInd + 2] = ind + 2; edges[eInd + 1] = ind + 0; edges[eInd + 0] = ind + 3;
//		eInd += 6;
//		ind += 4;
//	}
//	/* Face +X */
//	if((visFlags & FACE_RIGHT) == FACE_RIGHT) {
//		this->getTextureCoords( FACE_RIGHT, x, y );
//		GLuvrect rect = MagnetiteCore::Singleton->getTextureManager()->getBlockUVs( x, y );
//
//		data[ind + 0] = Renderer::vertex( this->getX() + 1.0f, this->getY() + fluidHeight, this->getZ() + 1.0f, // Coordinates
//								1.0f, 0.0f, 0.0f,
//								rect.x + rect.w, rect.y );
//		data[ind + 1] = Renderer::vertex( this->getX() + 1.0f, this->getY() - 0.0f, this->getZ() + 1.0f, // Coordinates
//								1.0f, 0.0f, 0.0f,
//								rect.x + rect.w, rect.y + rect.h );
//		data[ind + 2] = Renderer::vertex( this->getX() + 1.0f, this->getY() - 0.0f, this->getZ() - 0.0f, // Coordinates
//								1.0f, 0.0f, 0.0f,
//								rect.x, rect.y + rect.h );
//		data[ind + 3] = Renderer::vertex( this->getX() + 1.0f, this->getY() + fluidHeight, this->getZ() - 0.0f, // Coordinates
//								1.0f, 0.0f, 0.0f,
//								rect.x, rect.y );
//		edges[eInd + 0] = ind + 2; edges[eInd + 1] = ind + 1; edges[eInd + 2] = ind + 0;
//		edges[eInd + 3] = ind + 2; edges[eInd + 4] = ind + 0; edges[eInd + 5] = ind + 3;
//		eInd += 6;
//		ind += 4;
//	}
//	/* Face -Y */
//	if((visFlags & FACE_BOTTOM) == FACE_BOTTOM) {
//		this->getTextureCoords( FACE_BOTTOM, x, y );
//		GLuvrect rect = MagnetiteCore::Singleton->getTextureManager()->getBlockUVs( x, y );
//
//		data[ind + 0] = Renderer::vertex( this->getX() - 0.0f, this->getY() - 0.0f, this->getZ() + 1.0f, // Coordinates
//								0.0f, -1.0f, 0.0f,
//								rect.x, rect.y );
//		data[ind + 1] = Renderer::vertex( this->getX() - 0.0f, this->getY() - 0.0f, this->getZ() - 0.0f, // Coordinates
//								0.0f, -1.0f, 0.0f,
//								rect.x + rect.w, rect.y );
//		data[ind + 2] = Renderer::vertex( this->getX() + 1.0f, this->getY() - 0.0f, this->getZ() - 0.0f, // Coordinates
//								0.0f, -1.0f, 0.0f,
//								rect.x + rect.w, rect.y + rect.h );
//		data[ind + 3] = Renderer::vertex( this->getX() + 1.0f, this->getY() - 0.0f, this->getZ() + 1.0f, // Coordinates
//								0.0f, -1.0f, 0.0f,
//								rect.x, rect.y + rect.h );
//		edges[eInd + 0] = ind + 2; edges[eInd + 1] = ind + 1; edges[eInd + 2] = ind + 0;
//		edges[eInd + 3] = ind + 2; edges[eInd + 4] = ind + 0; edges[eInd + 5] = ind + 3;
//		eInd += 6;
//		ind += 4;
//	}
//	/* Face +Y */
//	if((visFlags & FACE_TOP) == FACE_TOP) {
//		this->getTextureCoords( FACE_TOP, x, y );
//		GLuvrect rect = MagnetiteCore::Singleton->getTextureManager()->getBlockUVs( x, y );
//
//		data[ind + 0] = Renderer::vertex( this->getX() - 0.0f, this->getY() + fluidHeight, this->getZ() + 1.0f, // Coordinates
//								0.0f, 1.0f, 0.0f,
//								rect.x, rect.y );
//		data[ind + 1] = Renderer::vertex( this->getX() + 1.0f, this->getY() + fluidHeight, this->getZ() + 1.0f, // Coordinates
//								0.0f, 1.0f, 0.0f,
//								rect.x + rect.w, rect.y );
//		data[ind + 2] = Renderer::vertex( this->getX() + 1.0f, this->getY() + fluidHeight, this->getZ() - 0.0f, // Coordinates
//								0.0f, 1.0f, 0.0f,
//								rect.x + rect.w, rect.y + rect.h );
//		data[ind + 3] = Renderer::vertex( this->getX() - 0.0f, this->getY() + fluidHeight, this->getZ() - 0.0f, // Coordinates
//								0.0f, 1.0f, 0.0f,
//								rect.x, rect.y + rect.h );
//		edges[eInd + 0] = ind + 2; edges[eInd + 1] = ind + 1; edges[eInd + 2] = ind + 0;
//		edges[eInd + 3] = ind + 2; edges[eInd + 4] = ind + 0; edges[eInd + 5] = ind + 3;
//		eInd += 6;
//		ind += 4;
//	}
//	/* Face -X */
//	if((visFlags & FACE_LEFT) == FACE_LEFT) {
//		this->getTextureCoords( FACE_LEFT, x, y );
//		GLuvrect rect = MagnetiteCore::Singleton->getTextureManager()->getBlockUVs( x, y );
//
//		data[ind + 0] = Renderer::vertex( this->getX() - 0.0f, this->getY() + fluidHeight, this->getZ() - 0.0f, // Coordinates
//								-1.0f, 0.0f, 0.0f,
//								rect.x + rect.w, rect.y );
//		data[ind + 1] = Renderer::vertex( this->getX() - 0.0f, this->getY() - 0.0f, this->getZ() - 0.0f, // Coordinates
//								-1.0f, 0.0f, 0.0f,
//								rect.x + rect.w, rect.y + rect.h );
//		data[ind + 2] = Renderer::vertex( this->getX() - 0.0f, this->getY() - 0.0f, this->getZ() + 1.0f, // Coordinates
//								-1.0f, 0.0f, 0.0f,
//								rect.x, rect.y + rect.h );
//		data[ind + 3] = Renderer::vertex( this->getX() - 0.0f, this->getY() + fluidHeight, this->getZ() + 1.0f, // Coordinates
//								-1.0f, 0.0f, 0.0f,
//								rect.x, rect.y );
//		edges[eInd + 0] = ind + 2; edges[eInd + 1] = ind + 1; edges[eInd + 2] = ind + 0;
//		edges[eInd + 3] = ind + 2; edges[eInd + 4] = ind + 0; edges[eInd + 5] = ind + 3;
//		eInd += 6;
//		ind += 4;
//	}
//}