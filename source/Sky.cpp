#include "Sky.h"
#include "Renderer.h"
#include "MagnetiteCore.h"
#include "TextureManager.h"

#include "glgeometry.h"
#include <util.h>
#include <ProgramResource.h>
#include <ResourceManager.h>

Sky::Sky()
: mTime( 0 ),
mMilliTime( 0 ),
mSkyTexture(NULL),
mSkyProgram(NULL)
{
	mGeom = new GLgeometry;
	mGeom->vertexCount = 8;
	mGeom->edgeCount = 6*6;
	mGeom->vertexData = new GLvertex[mGeom->vertexCount];
	mGeom->edgeData = new GLedge[mGeom->edgeCount];
	
	static float halfSize = 250.f;
	size_t v = 0, i = 0;
	
	mGeom->vertexData[v + 0] = GLgeometry::vertex( halfSize, halfSize, halfSize );
	mGeom->vertexData[v + 1] = GLgeometry::vertex( halfSize, halfSize,-halfSize );
	mGeom->vertexData[v + 2] = GLgeometry::vertex(-halfSize, halfSize, halfSize );
	mGeom->vertexData[v + 3] = GLgeometry::vertex(-halfSize, halfSize,-halfSize );
	
	mGeom->vertexData[v + 4] = GLgeometry::vertex( halfSize,-halfSize, halfSize );
	mGeom->vertexData[v + 5] = GLgeometry::vertex( halfSize,-halfSize,-halfSize );
	mGeom->vertexData[v + 6] = GLgeometry::vertex(-halfSize,-halfSize, halfSize );
	mGeom->vertexData[v + 7] = GLgeometry::vertex(-halfSize,-halfSize,-halfSize );
	
	
	
	/* +Y Face */
	mGeom->edgeData[i + 0] = v + 0; mGeom->edgeData[i + 1] = v + 1; mGeom->edgeData[i + 2] = v + 2;
	mGeom->edgeData[i + 3] = v + 2; mGeom->edgeData[i + 4] = v + 1; mGeom->edgeData[i + 5] = v + 3;
	i += 6;
	
	/* +X Face */
	mGeom->edgeData[i + 0] = v + 4; mGeom->edgeData[i + 1] = v + 1; mGeom->edgeData[i + 2] = v + 0;
	mGeom->edgeData[i + 3] = v + 1; mGeom->edgeData[i + 4] = v + 4; mGeom->edgeData[i + 5] = v + 5;
	i += 6;
	
	/* +Z Face */
	mGeom->edgeData[i + 0] = v + 0; mGeom->edgeData[i + 1] = v + 2; mGeom->edgeData[i + 2] = v + 4;
	mGeom->edgeData[i + 3] = v + 4; mGeom->edgeData[i + 4] = v + 2; mGeom->edgeData[i + 5] = v + 6;
	i += 6;

	/* -Y Face */
	mGeom->edgeData[i + 0] = v + 6; mGeom->edgeData[i + 1] = v + 5; mGeom->edgeData[i + 2] = v + 4;
	mGeom->edgeData[i + 3] = v + 7; mGeom->edgeData[i + 4] = v + 5; mGeom->edgeData[i + 5] = v + 6;
	i += 6;
	
	/* -X Face */
	mGeom->edgeData[i + 0] = v + 2; mGeom->edgeData[i + 1] = v + 3; mGeom->edgeData[i + 2] = v + 7;
	mGeom->edgeData[i + 3] = v + 2; mGeom->edgeData[i + 4] = v + 7; mGeom->edgeData[i + 5] = v + 6;
	i += 6;
	
	/* -Z Face */
	mGeom->edgeData[i + 0] = v + 5; mGeom->edgeData[i + 1] = v + 3; mGeom->edgeData[i + 2] = v + 1;
	mGeom->edgeData[i + 3] = v + 3; mGeom->edgeData[i + 4] = v + 5; mGeom->edgeData[i + 5] = v + 7;
	i += 6;

	mGeom->bindToBuffer();
	
	if( MagnetiteCore::Singleton->getTextureManager()->fetchTexture("../resources/sprites/sky.png") == NULL )
	{
		MagnetiteCore::Singleton->getTextureManager()->loadTexture("resources/sprites/sky.png");
	}
	
	mSkyTexture = MagnetiteCore::Singleton->getTextureManager()->fetchTexture("resources/sprites/sky.png");
	mSkyProgram = MagnetiteCore::Singleton->getResourceManager()->getResource<ProgramResource>("world_sky.prog");
	mSkyProgram->link();
}

Sky::~Sky()
{

}

void Sky::renderSky()
{
	mSkyProgram->makeActive();

	glEnable(GL_TEXTURE_2D);

	if(mSkyTexture != NULL) {
		glClientActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mSkyTexture->glID);
		auto txuI = mSkyProgram->getUniformLocation("skyDiffuse");
		glUniform1i(txuI, 0);
	}
	
	glBindBuffer( GL_ARRAY_BUFFER, mGeom->vertexBO );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mGeom->indexBO );

	// Get the in_vertex index.
	auto vtxI = mSkyProgram->getAttributeIndex("in_vertex");
	glEnableVertexAttribArray(vtxI);
	glVertexAttribPointer( vtxI, 3, GL_FLOAT, GL_FALSE, sizeof(GLvertex), BUFFER_OFFSET(0) );

	glDrawRangeElements(GL_TRIANGLES, 0, mGeom->vertexCount, mGeom->edgeCount, GL_UNSIGNED_SHORT, 0);
	
	glDisableVertexAttribArray(vtxI);
	
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	if(mSkyTexture != NULL) {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
		
	glDisable(GL_TEXTURE_2D);
	glUseProgram(0);
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

