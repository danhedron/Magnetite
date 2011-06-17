#include "BaseBlock.h"
#include "OpencraftCore.h"
#include "TextureManager.h"
#include "Renderer.h"
#include "WorldChunk.h"
#include <string>

BaseBlock::BaseBlock()
: mDataFlags( 0 ),
mViewFlags(0),
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

unsigned short BaseBlock::getX() {
	return (mDataFlags & BMASK_XPOS);
}

unsigned short BaseBlock::getY() {
	return ((mDataFlags & BMASK_YPOS)>>8);
}

unsigned short BaseBlock::getZ() {
	return ((mDataFlags & BMASK_ZPOS)>>4);
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

void BaseBlock::buildCubeData(size_t& ind, size_t& eInd, GLvertex* data, GLedge* edges)
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

	/* Face -Z */
	if((this->mViewFlags & FACE_BACK) == FACE_BACK ) {
		this->getTextureCoords( FACE_BACK, x, y );
		GLuvrect rect = OpencraftCore::Singleton->getTextureManager()->getBlockUVs( x, y );

		data[ind + 0] = Renderer::vertex( this->getX() + 1.0f, this->getY() + 1.0f,	this->getZ() + 1.0f, // Coordinates
								0.0f, 0.0f, -1.0f,
								rect.x, rect.y );
		data[ind + 1] = Renderer::vertex( this->getX() - 0.0f, this->getY() + 1.0f, this->getZ() + 1.0f, // Coordinates
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

		data[ind + 0] = Renderer::vertex( this->getX() + 1.0f, this->getY() + 1.0f, this->getZ() - 0.0f, // Coordinates
								0.0f, 0.0f, 1.0f,
								rect.x, rect.y );
		data[ind + 1] = Renderer::vertex( this->getX() - 0.0f, this->getY() + 1.0f, this->getZ() - 0.0f, // Coordinates
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

		data[ind + 0] = Renderer::vertex( this->getX() + 1.0f, this->getY() + 1.0f, this->getZ() + 1.0f, // Coordinates
								1.0f, 0.0f, 0.0f,
								rect.x + rect.w, rect.y );
		data[ind + 1] = Renderer::vertex( this->getX() + 1.0f, this->getY() - 0.0f, this->getZ() + 1.0f, // Coordinates
								1.0f, 0.0f, 0.0f,
								rect.x + rect.w, rect.y + rect.h );
		data[ind + 2] = Renderer::vertex( this->getX() + 1.0f, this->getY() - 0.0f, this->getZ() - 0.0f, // Coordinates
								1.0f, 0.0f, 0.0f,
								rect.x, rect.y + rect.h );
		data[ind + 3] = Renderer::vertex( this->getX() + 1.0f, this->getY() + 1.0f, this->getZ() - 0.0f, // Coordinates
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

		data[ind + 0] = Renderer::vertex( this->getX() - 0.0f, this->getY() + 1.0f, this->getZ() + 1.0f, // Coordinates
								0.0f, 1.0f, 0.0f,
								rect.x, rect.y );
		data[ind + 1] = Renderer::vertex( this->getX() + 1.0f, this->getY() + 1.0f, this->getZ() + 1.0f, // Coordinates
								0.0f, 1.0f, 0.0f,
								rect.x + rect.w, rect.y );
		data[ind + 2] = Renderer::vertex( this->getX() + 1.0f, this->getY() + 1.0f, this->getZ() - 0.0f, // Coordinates
								0.0f, 1.0f, 0.0f,
								rect.x + rect.w, rect.y + rect.h );
		data[ind + 3] = Renderer::vertex( this->getX() - 0.0f, this->getY() + 1.0f, this->getZ() - 0.0f, // Coordinates
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

		data[ind + 0] = Renderer::vertex( this->getX() - 0.0f, this->getY() + 1.0f, this->getZ() - 0.0f, // Coordinates
								-1.0f, 0.0f, 0.0f,
								rect.x + rect.w, rect.y );
		data[ind + 1] = Renderer::vertex( this->getX() - 0.0f, this->getY() - 0.0f, this->getZ() - 0.0f, // Coordinates
								-1.0f, 0.0f, 0.0f,
								rect.x + rect.w, rect.y + rect.h );
		data[ind + 2] = Renderer::vertex( this->getX() - 0.0f, this->getY() - 0.0f, this->getZ() + 1.0f, // Coordinates
								-1.0f, 0.0f, 0.0f,
								rect.x, rect.y + rect.h );
		data[ind + 3] = Renderer::vertex( this->getX() - 0.0f, this->getY() + 1.0f, this->getZ() + 1.0f, // Coordinates
								-1.0f, 0.0f, 0.0f,
								rect.x, rect.y );
		edges[eInd + 0] = ind + 2; edges[eInd + 1] = ind + 1; edges[eInd + 2] = ind + 0;
		edges[eInd + 3] = ind + 2; edges[eInd + 4] = ind + 0; edges[eInd + 5] = ind + 3;
		eInd += 6;
		ind += 4;
	}
}