#include "Sky.h"
#include "Renderer.h"
#include "MagnetiteCore.h"
#include "TextureManager.h"

Sky::Sky()
: mTime( 0 ),
mMilliTime( 0 )
{
	mGeom = new GLgeometry;
	mGeom->vertexData = new GLvertex[20];
	mGeom->vertexCount = 20;
	mGeom->edgeCount = 20 * 6;
	mGeom->edgeData = new GLedge[20*6];
	size_t face = 0;
	size_t edge = 0;
	
	/* Face +Y */
	mGeom->vertexData[face+ 0] = Renderer::vertex( 250.f, 250.f, 250.f,
											0.f, -1.f, 0.f);
	mGeom->vertexData[face+ 1] = Renderer::vertex( 250.f, 250.f, -250.f,
											0.f, -1.f, 0.f);
	mGeom->vertexData[face+ 2] = Renderer::vertex( -250.f, 250.f, -250.f,
											0.f, -1.f, 0.f);
	mGeom->vertexData[face+ 3] = Renderer::vertex( -250.f, 250.f, 250.f,
											0.f, -1.f, 0.f);
	mGeom->edgeData[edge+0] = face + 0; mGeom->edgeData[edge+1] = face + 1; mGeom->edgeData[edge+2] = face + 2;
	mGeom->edgeData[edge+3] = face + 0; mGeom->edgeData[edge+4] = face + 2; mGeom->edgeData[edge+5] = face + 3;
	face += 4;
	edge += 6;

	/* Face +X */
	mGeom->vertexData[face+ 0] = Renderer::vertex( 250.f, -250.f, 250.f,
											-1.f, 0.f, 0.f);
	mGeom->vertexData[face+ 1] = Renderer::vertex( 250.f, -250.f, -250.f,
											-1.f, 0.f, 0.f);
	mGeom->vertexData[face+ 2] = Renderer::vertex( 250.f, 250.f, -250.f,
											-1.f, 0.f, 0.f);
	mGeom->vertexData[face+ 3] = Renderer::vertex( 250.f, 250.f, 250.f,
											-1.f, 0.f, 0.f);
	mGeom->edgeData[edge+0] = face + 0; mGeom->edgeData[edge+1] = face + 1; mGeom->edgeData[edge+2] = face + 2;
	mGeom->edgeData[edge+3] = face + 0; mGeom->edgeData[edge+4] = face + 2; mGeom->edgeData[edge+5] = face + 3;
	face += 4;
	edge += 6;

	/* Face -X */
	mGeom->vertexData[face+ 0] = Renderer::vertex( -250.f, 250.f, 250.f,
											1.f, 0.f, 0.f);
	mGeom->vertexData[face+ 1] = Renderer::vertex( -250.f, 250.f, -250.f,
											1.f, 0.f, 0.f);
	mGeom->vertexData[face+ 2] = Renderer::vertex( -250.f, -250.f, -250.f,
											1.f, 0.f, 0.f);
	mGeom->vertexData[face+ 3] = Renderer::vertex( -250.f, -250.f, 250.f,
											1.f, 0.f, 0.f);
	mGeom->edgeData[edge+0] = face + 0; mGeom->edgeData[edge+1] = face + 1; mGeom->edgeData[edge+2] = face + 2;
	mGeom->edgeData[edge+3] = face + 0; mGeom->edgeData[edge+4] = face + 2; mGeom->edgeData[edge+5] = face + 3;
	face += 4;
	edge += 6;
		
	/* Face +Z */
	mGeom->vertexData[face+ 0] = Renderer::vertex( 250.f, 250.f, 250.f,
											0.f, 0.f, -1.f);
	mGeom->vertexData[face+ 1] = Renderer::vertex( -250.f, 250.f, 250.f,
											0.f, 0.f, -1.f);
	mGeom->vertexData[face+ 2] = Renderer::vertex( -250.f, -250.f, 250.f,
											0.f, 0.f, -1.f);
	mGeom->vertexData[face+ 3] = Renderer::vertex( 250.f, -250.f, 250.f,
											0.f, 0.f, -1.f);
	mGeom->edgeData[edge+0] = face + 0; mGeom->edgeData[edge+1] = face + 1; mGeom->edgeData[edge+2] = face + 2;
	mGeom->edgeData[edge+3] = face + 0; mGeom->edgeData[edge+4] = face + 2; mGeom->edgeData[edge+5] = face + 3;
	face += 4;
	edge += 6;

	/* Face -Z */
	mGeom->vertexData[face+ 0] = Renderer::vertex( 250.f, -250.f, -250.f,
											0.f, 0.f, 1.f);
	mGeom->vertexData[face+ 1] = Renderer::vertex( -250.f, -250.f, -250.f,
											0.f, 0.f, 1.f);
	mGeom->vertexData[face+ 2] = Renderer::vertex( -250.f, 250.f, -250.f,
											0.f, 0.f, 1.f);
	mGeom->vertexData[face+ 3] = Renderer::vertex( 250.f, 250.f, -250.f,
											0.f, 0.f, 1.f);
	mGeom->edgeData[edge+0] = face + 0; mGeom->edgeData[edge+1] = face + 1; mGeom->edgeData[edge+2] = face + 2;
	mGeom->edgeData[edge+3] = face + 0; mGeom->edgeData[edge+4] = face + 2; mGeom->edgeData[edge+5] = face + 3;
	face += 4;
	edge += 6;

	for( size_t i = 0; i < mGeom->vertexCount; i++ )
	{
		mGeom->vertexData[i].u0 = 0.5f;
		mGeom->vertexData[i].v0 = 0.f;
	}
}

Sky::~Sky()
{

}

void Sky::renderSky()
{
	if( MagnetiteCore::Singleton->getTextureManager()->fetchTexture("../resources/sprites/sky.png") == NULL )
		MagnetiteCore::Singleton->getTextureManager()->loadTexture("../resources/sprites/sky.png");

	glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	GLtexture* tex = MagnetiteCore::Singleton->getTextureManager()->fetchTexture("../resources/sprites/sky.png");
	
	if(tex != 0) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, tex->glID);
	}

	glVertexPointer(3, GL_FLOAT, sizeof(GLvertex), &(mGeom->vertexData[0].x));
	//glNormalPointer(GL_FLOAT, sizeof(GLvertex), &(mGeom->vertexData[0].nx));
	glTexCoordPointer(2, GL_FLOAT, sizeof(GLvertex), &(mGeom->vertexData[0].u0));
	glClientActiveTexture(GL_TEXTURE0);

	//glDrawRangeElements(GL_TRIANGLES, 0, mGeom->vertexCount, mGeom->edgeCount, GL_UNSIGNED_SHORT, mGeom->edgeData);

	if(tex) {
		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

float Sky::timeUV()
{
	return ((float)(mTime % DAY_LENGTH)) / ((float)DAY_LENGTH);
}

void Sky::setTime( size_t time )
{
	mTime = time;
}

size_t Sky::getTime()
{
	return mTime;
}

void Sky::update( float dt )
{
	mMilliTime += dt;
	while( mMilliTime > 1.f ) {
		mTime++;
		mMilliTime -= 1.f;
	}

	/* Update geometry UVs with current time */
	for( size_t i = 0; i < mGeom->vertexCount; i++ )
	{
		mGeom->vertexData[i].u0 = timeUV();
		mGeom->vertexData[i].v0 = 0.f;
	}
}

