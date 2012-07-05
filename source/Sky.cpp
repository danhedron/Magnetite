#include "Sky.h"
#include "Renderer.h"
#include "MagnetiteCore.h"

#include "Geometry.h"
#include <util.h>
#include <ProgramResource.h>
#include <Texture.h>
#include <ResourceManager.h>

Sky::Sky()
: mTime( 0 ),
mMilliTime( 0 ),
mSkyTexture(NULL),
mSkyProgram(NULL)
{
	mGeom = new MeshGeometry;
	mGeom->vertexCount = 8;
	mGeom->edgeCount = 6*6;
	mGeom->vertexData = new GeometryVertex[mGeom->vertexCount];
	mGeom->edgeData = new GLedge[mGeom->edgeCount];
	
	static float halfSize = 0.5f;
	size_t v = 0, i = 0;
	
	mGeom->vertexData[v + 0] = Geometry::vertex( halfSize, halfSize, halfSize );
	mGeom->vertexData[v + 1] = Geometry::vertex( halfSize, halfSize,-halfSize );
	mGeom->vertexData[v + 2] = Geometry::vertex(-halfSize, halfSize, halfSize );
	mGeom->vertexData[v + 3] = Geometry::vertex(-halfSize, halfSize,-halfSize );
	
	mGeom->vertexData[v + 4] = Geometry::vertex( halfSize,-halfSize, halfSize );
	mGeom->vertexData[v + 5] = Geometry::vertex( halfSize,-halfSize,-halfSize );
	mGeom->vertexData[v + 6] = Geometry::vertex(-halfSize,-halfSize, halfSize );
	mGeom->vertexData[v + 7] = Geometry::vertex(-halfSize,-halfSize,-halfSize );
	
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
	
	mSkyTexture = MagnetiteCore::Singleton->getResourceManager()->getResource<Texture>("sky.png");
	mSkyTexture->setFilter(GL_LINEAR);
	mSkyTexture->load();
	mSkyProgram = MagnetiteCore::Singleton->getResourceManager()->getResource<ProgramResource>("world_sky.prog");
	mSkyProgram->link();
}

Sky::~Sky()
{

}

void Sky::renderSky()
{
	mSkyProgram->makeActive();
	
	// Set up the uniform for the world diffuse texture.
	GLint samplerLocation = mSkyProgram->getUniformLocation("skyDiffuse");
	if( samplerLocation != -1 )
	{
		glEnable(GL_TEXTURE_2D);
		glUniform1i( samplerLocation, 0 );
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture(GL_TEXTURE_2D, mSkyTexture->getName());
	}
	
	glBindBuffer( GL_ARRAY_BUFFER, mGeom->vertexBO );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mGeom->indexBO );

	mGeom->bindVertexAttributes( mSkyProgram );
	
	glDrawRangeElements(GL_TRIANGLES, 0, mGeom->vertexCount, mGeom->edgeCount, GL_UNSIGNED_SHORT, 0);
	
	
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	if(mSkyTexture != NULL) {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
		
	glDisable(GL_TEXTURE_2D);
	
	mSkyProgram->deactivate();
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

