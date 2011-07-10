#include "BaseBlock.h"
#include "OpencraftCore.h"
#include "TextureManager.h"
#include "Renderer.h"
#include "WorldChunk.h"
#include "World.h"
#include <string>

BaseBlock::BaseBlock()
: mDataFlags( 0 ),
mChunk( NULL )
{
}

BaseBlock::~BaseBlock(void)
{
}

void BaseBlock::_setPosition( unsigned short x, unsigned  short y, unsigned short z)
{
	mDataFlags = ( x | (z<<4) | (y<<8) );
}

void BaseBlock::connectedChange( short face )
{

}

void BaseBlock::_setChunk( WorldChunk* chnk )
{
	mChunk = chnk;
}

short BaseBlock::getX() {
	return (mDataFlags & BMASK_XPOS);
}

short BaseBlock::getY() {
	return ((mDataFlags & BMASK_YPOS)>>8);
}

short BaseBlock::getZ() {
	return ((mDataFlags & BMASK_ZPOS)>>4);
}

void BaseBlock::updateVisFlags(unsigned int flags)
{
	mDataFlags = (mDataFlags & ~BMASK_VISFLAGS) | (mDataFlags & ~(FACE_ALL<<15));
	mDataFlags = mDataFlags | (flags<<15);
}

unsigned short BaseBlock::getVisFlags()
{
	return ((mDataFlags & BMASK_VISFLAGS)>>15);
}

void BaseBlock::getTextureCoords( short face, short &x, short &y )
{
	x = 0;
	y = 0;
}

void BaseBlock::setPosition( long x, long y, long z )
{
	if( mChunk )
		mChunk->_blockMoved( this, x, y, z );
	_setPosition(x,y,z);
}

void BaseBlock::setPosition( const Vector3& vec)
{
	setPosition( vec.x, vec.y, vec.z );
}

void BaseBlock::appendToStream( std::ofstream& stream )
{
	stream.write( (char *) &mDataFlags, sizeof( mDataFlags ) );	
}

void BaseBlock::readFromStream( std::ifstream& stream )
{
	stream.read( (char *) &mDataFlags, sizeof( mDataFlags ) );	
}

void BaseBlock::buildCubeData(size_t& ind, size_t& eInd, GLvertex* data, GLedge* edges)
{
	if( mChunk == NULL ) {
		Util::log("Can't generate geometry without a chunk?");
		return;
	}
	short x = 0, y = 0;

	short visFlags = getVisFlags();

	/* Face -Z */
	if((visFlags & FACE_BACK) == FACE_BACK ) {
		this->getTextureCoords( FACE_BACK, x, y );
		GLuvrect rect = OpencraftCore::Singleton->getTextureManager()->getBlockUVs( x, y );

		float color = World::getLightColor( mChunk->getLightLevel( getX(), getY(), getZ()+1 ) );
		data[ind + 0] = Renderer::vertex( this->getX() + 1.0f, this->getY() + 1.0f,	this->getZ() + 1.0f, // Coordinates
								0.0f, 0.0f, -1.0f,
								rect.x, rect.y,
								color,color,color);
		data[ind + 1] = Renderer::vertex( this->getX() - 0.0f, this->getY() + 1.0f, this->getZ() + 1.0f, // Coordinates
								0.0f, 0.0f, -1.0f,
								rect.x + rect.w, rect.y,
								color,color,color );
		data[ind + 2] = Renderer::vertex( this->getX() - 0.0f, this->getY() - 0.0f, this->getZ() + 1.0f, // Coordinates
								0.0f, 0.0f, -1.0f,
								rect.x + rect.w, rect.y + rect.h,
								color,color,color );
		data[ind + 3] = Renderer::vertex( this->getX() + 1.0f, this->getY() - 0.0f, this->getZ() + 1.0f, // Coordinates
								0.0f, 0.0f, -1.0f,
								rect.x, rect.y + rect.h,
								color,color,color );
		edges[eInd + 0] = ind + 2; edges[eInd + 1] = ind + 1; edges[eInd + 2] = ind + 0;
		edges[eInd + 3] = ind + 2; edges[eInd + 4] = ind + 0; edges[eInd + 5] = ind + 3;
		eInd += 6;
 		ind += 4;
	}
	/* Face +Z */
	if((visFlags & FACE_FORWARD) == FACE_FORWARD ) {
		this->getTextureCoords( FACE_FORWARD, x, y );
		GLuvrect rect = OpencraftCore::Singleton->getTextureManager()->getBlockUVs( x, y );

		float color = World::getLightColor( mChunk->getLightLevel( getX(), getY(), getZ()-1 ) );
		data[ind + 0] = Renderer::vertex( this->getX() + 1.0f, this->getY() + 1.0f, this->getZ() - 0.0f, // Coordinates
								0.0f, 0.0f, 1.0f,
								rect.x, rect.y,
								color,color,color );
		data[ind + 1] = Renderer::vertex( this->getX() - 0.0f, this->getY() + 1.0f, this->getZ() - 0.0f, // Coordinates
								0.0f, 0.0f, 1.0f,
								rect.x + rect.w, rect.y,
								color,color,color );
		data[ind + 2] = Renderer::vertex( this->getX() - 0.0f, this->getY() - 0.0f, this->getZ() - 0.0f, // Coordinates
								0.0f, 0.0f, 1.0f,
								rect.x + rect.w, rect.y + rect.h,
								color,color,color );
		data[ind + 3] = Renderer::vertex( this->getX() + 1.0f, this->getY() - 0.0f, this->getZ() - 0.0f, // Coordinates
								0.0f, 0.0f, 1.0f,
								rect.x, rect.y + rect.h,
								color,color,color );
		edges[eInd + 5] = ind + 2; edges[eInd + 4] = ind + 1; edges[eInd + 3] = ind + 0;
		edges[eInd + 2] = ind + 2; edges[eInd + 1] = ind + 0; edges[eInd + 0] = ind + 3;
		eInd += 6;
		ind += 4;
	}
	/* Face +X */
	if((visFlags & FACE_RIGHT) == FACE_RIGHT) {
		this->getTextureCoords( FACE_RIGHT, x, y );
		GLuvrect rect = OpencraftCore::Singleton->getTextureManager()->getBlockUVs( x, y );

		float color = World::getLightColor( mChunk->getLightLevel( getX()+1, getY(), getZ() ) );
		data[ind + 0] = Renderer::vertex( this->getX() + 1.0f, this->getY() + 1.0f, this->getZ() + 1.0f, // Coordinates
								1.0f, 0.0f, 0.0f,
								rect.x + rect.w, rect.y,
								color,color,color );
		data[ind + 1] = Renderer::vertex( this->getX() + 1.0f, this->getY() - 0.0f, this->getZ() + 1.0f, // Coordinates
								1.0f, 0.0f, 0.0f,
								rect.x + rect.w, rect.y + rect.h,
								color,color,color );
		data[ind + 2] = Renderer::vertex( this->getX() + 1.0f, this->getY() - 0.0f, this->getZ() - 0.0f, // Coordinates
								1.0f, 0.0f, 0.0f,
								rect.x, rect.y + rect.h,
								color,color,color );
		data[ind + 3] = Renderer::vertex( this->getX() + 1.0f, this->getY() + 1.0f, this->getZ() - 0.0f, // Coordinates
								1.0f, 0.0f, 0.0f,
								rect.x, rect.y,
								color,color,color );
		edges[eInd + 0] = ind + 2; edges[eInd + 1] = ind + 1; edges[eInd + 2] = ind + 0;
		edges[eInd + 3] = ind + 2; edges[eInd + 4] = ind + 0; edges[eInd + 5] = ind + 3;
		eInd += 6;
		ind += 4;
	}
	/* Face -Y */
	if((visFlags & FACE_BOTTOM) == FACE_BOTTOM) {
		this->getTextureCoords( FACE_BOTTOM, x, y );
		GLuvrect rect = OpencraftCore::Singleton->getTextureManager()->getBlockUVs( x, y );

		float color = World::getLightColor( mChunk->getLightLevel( getX(), getY()-1, getZ() ) );
		data[ind + 0] = Renderer::vertex( this->getX() - 0.0f, this->getY() - 0.0f, this->getZ() + 1.0f, // Coordinates
								0.0f, -1.0f, 0.0f,
								rect.x, rect.y,
								color,color,color );
		data[ind + 1] = Renderer::vertex( this->getX() - 0.0f, this->getY() - 0.0f, this->getZ() - 0.0f, // Coordinates
								0.0f, -1.0f, 0.0f,
								rect.x + rect.w, rect.y,
								color,color,color );
		data[ind + 2] = Renderer::vertex( this->getX() + 1.0f, this->getY() - 0.0f, this->getZ() - 0.0f, // Coordinates
								0.0f, -1.0f, 0.0f,
								rect.x + rect.w, rect.y + rect.h,
								color,color,color );
		data[ind + 3] = Renderer::vertex( this->getX() + 1.0f, this->getY() - 0.0f, this->getZ() + 1.0f, // Coordinates
								0.0f, -1.0f, 0.0f,
								rect.x, rect.y + rect.h,
								color,color,color );
		edges[eInd + 0] = ind + 2; edges[eInd + 1] = ind + 1; edges[eInd + 2] = ind + 0;
		edges[eInd + 3] = ind + 2; edges[eInd + 4] = ind + 0; edges[eInd + 5] = ind + 3;
		eInd += 6;
		ind += 4;
	}
	/* Face +Y */
	if((visFlags & FACE_TOP) == FACE_TOP) {
		this->getTextureCoords( FACE_TOP, x, y );
		GLuvrect rect = OpencraftCore::Singleton->getTextureManager()->getBlockUVs( x, y );

		float color = World::getLightColor( mChunk->getLightLevel( getX(), getY()+1, getZ() ) );
		data[ind + 0] = Renderer::vertex( this->getX() - 0.0f, this->getY() + 1.0f, this->getZ() + 1.0f, // Coordinates
								0.0f, 1.0f, 0.0f,
								rect.x, rect.y,
								color,color,color );
		data[ind + 1] = Renderer::vertex( this->getX() + 1.0f, this->getY() + 1.0f, this->getZ() + 1.0f, // Coordinates
								0.0f, 1.0f, 0.0f,
								rect.x + rect.w, rect.y,
								color,color,color );
		data[ind + 2] = Renderer::vertex( this->getX() + 1.0f, this->getY() + 1.0f, this->getZ() - 0.0f, // Coordinates
								0.0f, 1.0f, 0.0f,
								rect.x + rect.w, rect.y + rect.h,
								color,color,color );
		data[ind + 3] = Renderer::vertex( this->getX() - 0.0f, this->getY() + 1.0f, this->getZ() - 0.0f, // Coordinates
								0.0f, 1.0f, 0.0f,
								rect.x, rect.y + rect.h,
								color,color,color );
		edges[eInd + 0] = ind + 2; edges[eInd + 1] = ind + 1; edges[eInd + 2] = ind + 0;
		edges[eInd + 3] = ind + 2; edges[eInd + 4] = ind + 0; edges[eInd + 5] = ind + 3;
		eInd += 6;
		ind += 4;
	}
	/* Face -X */
	if((visFlags & FACE_LEFT) == FACE_LEFT) {
		this->getTextureCoords( FACE_LEFT, x, y );
		GLuvrect rect = OpencraftCore::Singleton->getTextureManager()->getBlockUVs( x, y );

		float color = World::getLightColor( mChunk->getLightLevel( getX()-1, getY(), getZ() ) );
		data[ind + 0] = Renderer::vertex( this->getX() - 0.0f, this->getY() + 1.0f, this->getZ() - 0.0f, // Coordinates
								-1.0f, 0.0f, 0.0f,
								rect.x + rect.w, rect.y,
								color,color,color );
		data[ind + 1] = Renderer::vertex( this->getX() - 0.0f, this->getY() - 0.0f, this->getZ() - 0.0f, // Coordinates
								-1.0f, 0.0f, 0.0f,
								rect.x + rect.w, rect.y + rect.h,
								color,color,color );
		data[ind + 2] = Renderer::vertex( this->getX() - 0.0f, this->getY() - 0.0f, this->getZ() + 1.0f, // Coordinates
								-1.0f, 0.0f, 0.0f,
								rect.x, rect.y + rect.h,
								color,color,color );
		data[ind + 3] = Renderer::vertex( this->getX() - 0.0f, this->getY() + 1.0f, this->getZ() + 1.0f, // Coordinates
								-1.0f, 0.0f, 0.0f,
								rect.x, rect.y,
								color,color,color );
		edges[eInd + 0] = ind + 2; edges[eInd + 1] = ind + 1; edges[eInd + 2] = ind + 0;
		edges[eInd + 3] = ind + 2; edges[eInd + 4] = ind + 0; edges[eInd + 5] = ind + 3;
		eInd += 6;
		ind += 4;
	}
}