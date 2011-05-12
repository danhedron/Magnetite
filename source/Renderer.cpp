#include "Renderer.h"
#include "WorldChunk.h"
#include "BaseBlock.h"
#include "OpencraftCore.h"
#include "TextureManager.h"

#include "util.h"
#include "assert.h"

Renderer::Renderer(void)
: totalTime( 0 ),
chunkVbo( 0 ),
mScrWidth( 0 ),
mScrHeight( 0 ),
mBlRendered( 0 ),
mBlTotal( 0 ),
mRenderMode( RENDER_SOLID ),
mFpsAvg( 0 )
{
}

Renderer::~Renderer(void)
{
	if( chunkVbo != 0 )
		glDeleteBuffersARB(1, &chunkVbo);
}

void Renderer::initialize(int *argc, char **argv)
{
	// Set color and depth clear value
	glClearDepth(1.f);
	glClearColor(0.f, 0.f, 0.f, 0.f);

	// Enable Z-buffer read and write
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	// Setup a perspective projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.f, 1.f, 0.1f, 500.f);

	glutInit( argc, argv );
	Vector3 vec(0, 10.0f, 10.0f);
	mCamera.setPosition(vec);
}

void Renderer::resizeViewport(size_t x, size_t y, size_t w, size_t h)
{
	glViewport(x, y, w, h);
	mScrWidth = w;
	mScrHeight = h;
	float width = w;
	float height = h;
	float aspect = width/height;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.f, aspect, 0.1f, 500.f);
	Util::log( "Window Resized: " + Util::toString(w) + "x" + Util::toString(h) );

}

void Renderer::enable2D()
{
	//Switch matrix
	glMatrixMode(GL_PROJECTION);
	// Clear to identity
	glPushMatrix();
	glLoadIdentity();
	// Set perspective & coordinate fix
	gluOrtho2D(0.0, mScrWidth, 0.0, mScrHeight);
	glTranslatef(0, mScrHeight, 0);
	glScalef(1, -1, 1); // Flip Y coordinates
	// Switch Back
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

void Renderer::disable2D()
{
	// Just pop 2d matrix.
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void Renderer::setRenderMode( size_t rendermode )
{
	mRenderMode = rendermode;
}

size_t Renderer::getRenderMode()
{
	return mRenderMode;
}

Camera& Renderer::getCamera()
{
	return mCamera;
}

GLvertex Renderer::vertex(float x, float y, float z, float nx, float ny, float nz, float u, float v, float w)
{
	GLvertex vert = { x, y, z, nx, ny, nz, u, v };
	return vert;
}

void Renderer::buildCubeData(BaseBlock* block, size_t& ind, size_t& eInd, GLvertex* data, GLedge* edges)
{
	GLuvrect rect = OpencraftCore::Singleton->getTextureManager()->getBlockUVs( block->getTextureX(), block->getTextureY() );
	block->vertexIndex = ind;

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
	if((block->mViewFlags & VIS_BACK) == VIS_BACK ) {
		//if( block->getY() >= 4 ) {
		//	Util::log("Above surface: " + block->getType() + " " + Util::toString(rect.x) + " " + Util::toString(rect.y));
		//}
		data[ind + 0] = vertex( block->getX() + 0.5f, block->getY() + 0.5f,	block->getZ() + 0.5f, // Coordinates
								0.0f, 0.0f, -1.0f,
								rect.x, rect.y );
		data[ind + 1] = vertex( block->getX() - 0.5f, block->getY() + 0.5f, block->getZ() + 0.5f, // Coordinates
								0.0f, 0.0f, -1.0f,
								rect.x + rect.w, rect.y );
		data[ind + 2] = vertex( block->getX() - 0.5f, block->getY() - 0.5f, block->getZ() + 0.5f, // Coordinates
								0.0f, 0.0f, -1.0f,
								rect.x + rect.w, rect.y + rect.h );
		data[ind + 3] = vertex( block->getX() + 0.5f, block->getY() - 0.5f, block->getZ() + 0.5f, // Coordinates
								0.0f, 0.0f, -1.0f,
								rect.x, rect.y + rect.h );
		edges[eInd + 0] = ind + 2; edges[eInd + 1] = ind + 1; edges[eInd + 2] = ind + 0;
		edges[eInd + 3] = ind + 2; edges[eInd + 4] = ind + 0; edges[eInd + 5] = ind + 3;
		eInd += 6;
 		ind += 4;
	}
	/* Face +Z */
	if((block->mViewFlags & VIS_FORWARD) == VIS_FORWARD ) {
		data[ind + 0] = vertex( block->getX() + 0.5f, block->getY() + 0.5f, block->getZ() - 0.5f, // Coordinates
								0.0f, 0.0f, 1.0f,
								rect.x, rect.y );
		data[ind + 1] = vertex( block->getX() - 0.5f, block->getY() + 0.5f, block->getZ() - 0.5f, // Coordinates
								0.0f, 0.0f, 1.0f,
								rect.x + rect.w, rect.y );
		data[ind + 2] = vertex( block->getX() - 0.5f, block->getY() - 0.5f, block->getZ() - 0.5f, // Coordinates
								0.0f, 0.0f, 1.0f,
								rect.x + rect.w, rect.y + rect.h );
		data[ind + 3] = vertex( block->getX() + 0.5f, block->getY() - 0.5f, block->getZ() - 0.5f, // Coordinates
								0.0f, 0.0f, 1.0f,
								rect.x, rect.y + rect.h );
		edges[eInd + 5] = ind + 2; edges[eInd + 4] = ind + 1; edges[eInd + 3] = ind + 0;
		edges[eInd + 2] = ind + 2; edges[eInd + 1] = ind + 0; edges[eInd + 0] = ind + 3;
		eInd += 6;
		ind += 4;
	}
	/* Face +X */
	if((block->mViewFlags & VIS_RIGHT) == VIS_RIGHT) {
		data[ind + 0] = vertex( block->getX() + 0.5f, block->getY() + 0.5f, block->getZ() + 0.5f, // Coordinates
								1.0f, 0.0f, 0.0f,
								rect.x, rect.y );
		data[ind + 1] = vertex( block->getX() + 0.5f, block->getY() - 0.5f, block->getZ() + 0.5f, // Coordinates
								1.0f, 0.0f, 0.0f,
								rect.x + rect.w, rect.y );
		data[ind + 2] = vertex( block->getX() + 0.5f, block->getY() - 0.5f, block->getZ() - 0.5f, // Coordinates
								1.0f, 0.0f, 0.0f,
								rect.x + rect.w, rect.y + rect.h );
		data[ind + 3] = vertex( block->getX() + 0.5f, block->getY() + 0.5f, block->getZ() - 0.5f, // Coordinates
								1.0f, 0.0f, 0.0f,
								rect.x, rect.y + rect.h );
		edges[eInd + 0] = ind + 2; edges[eInd + 1] = ind + 1; edges[eInd + 2] = ind + 0;
		edges[eInd + 3] = ind + 2; edges[eInd + 4] = ind + 0; edges[eInd + 5] = ind + 3;
		eInd += 6;
		ind += 4;
	}
	/* Face -Y */
	if((block->mViewFlags & VIS_BOTTOM) == VIS_BOTTOM) {
		data[ind + 0] = vertex( block->getX() - 0.5f, block->getY() - 0.5f, block->getZ() + 0.5f, // Coordinates
								0.0f, -1.0f, 0.0f,
								rect.x, rect.y );
		data[ind + 1] = vertex( block->getX() - 0.5f, block->getY() - 0.5f, block->getZ() - 0.5f, // Coordinates
								0.0f, -1.0f, 0.0f,
								rect.x + rect.w, rect.y );
		data[ind + 2] = vertex( block->getX() + 0.5f, block->getY() - 0.5f, block->getZ() - 0.5f, // Coordinates
								0.0f, -1.0f, 0.0f,
								rect.x + rect.w, rect.y + rect.h );
		data[ind + 3] = vertex( block->getX() + 0.5f, block->getY() - 0.5f, block->getZ() + 0.5f, // Coordinates
								0.0f, -1.0f, 0.0f,
								rect.x, rect.y + rect.h );
		edges[eInd + 0] = ind + 2; edges[eInd + 1] = ind + 1; edges[eInd + 2] = ind + 0;
		edges[eInd + 3] = ind + 2; edges[eInd + 4] = ind + 0; edges[eInd + 5] = ind + 3;
		eInd += 6;
		ind += 4;
	}
	/* Face +Y */
	if((block->mViewFlags & VIS_TOP) == VIS_TOP) {
		data[ind + 0] = vertex( block->getX() - 0.5f, block->getY() + 0.5f, block->getZ() + 0.5f, // Coordinates
								0.0f, 1.0f, 0.0f,
								rect.x, rect.y );
		data[ind + 1] = vertex( block->getX() + 0.5f, block->getY() + 0.5f, block->getZ() + 0.5f, // Coordinates
								0.0f, 1.0f, 0.0f,
								rect.x + rect.w, rect.y );
		data[ind + 2] = vertex( block->getX() + 0.5f, block->getY() + 0.5f, block->getZ() - 0.5f, // Coordinates
								0.0f, 1.0f, 0.0f,
								rect.x + rect.w, rect.y + rect.h );
		data[ind + 3] = vertex( block->getX() - 0.5f, block->getY() + 0.5f, block->getZ() - 0.5f, // Coordinates
								0.0f, 1.0f, 0.0f,
								rect.x, rect.y + rect.h );
		edges[eInd + 0] = ind + 2; edges[eInd + 1] = ind + 1; edges[eInd + 2] = ind + 0;
		edges[eInd + 3] = ind + 2; edges[eInd + 4] = ind + 0; edges[eInd + 5] = ind + 3;
		eInd += 6;
		ind += 4;
	}
	/* Face -X */
	if((block->mViewFlags & VIS_LEFT) == VIS_LEFT) {
		data[ind + 0] = vertex( block->getX() - 0.5f, block->getY() + 0.5f, block->getZ() - 0.5f, // Coordinates
								-1.0f, 0.0f, 0.0f,
								rect.x, rect.y );
		data[ind + 1] = vertex( block->getX() - 0.5f, block->getY() - 0.5f, block->getZ() - 0.5f, // Coordinates
								-1.0f, 0.0f, 0.0f,
								rect.x + rect.w, rect.y );
		data[ind + 2] = vertex( block->getX() - 0.5f, block->getY() - 0.5f, block->getZ() + 0.5f, // Coordinates
								-1.0f, 0.0f, 0.0f,
								rect.x + rect.w, rect.y + rect.h );
		data[ind + 3] = vertex( block->getX() - 0.5f, block->getY() + 0.5f, block->getZ() + 0.5f, // Coordinates
								-1.0f, 0.0f, 0.0f,
								rect.x, rect.y + rect.h );
		edges[eInd + 0] = ind + 2; edges[eInd + 1] = ind + 1; edges[eInd + 2] = ind + 0;
		edges[eInd + 3] = ind + 2; edges[eInd + 4] = ind + 0; edges[eInd + 5] = ind + 3;
		eInd += 6;
		ind += 4;
	}
}

void Renderer::render(double dt, std::vector<WorldChunk*> &chunks)
{
	totalTime += dt;
	mFpsAvg = (mFpsAvg + (1/dt)) / 2;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);

	GLtexture* tex = OpencraftCore::Singleton->getTextureManager()->fetchTexture("../resources/sprites/world.png");
	if(mRenderMode == RENDER_WIRE)
		tex = OpencraftCore::Singleton->getTextureManager()->fetchTexture("../resources/sprites/vistest.png");
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glColor3f(1,1,1);

	
	if( tex != 0 )
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, tex->glID);
	}

	for(std::vector<WorldChunk*>::iterator chunk = chunks.begin();
		chunk != chunks.end();
		++chunk)
	{
		// Sort out view Matrix.
		glLoadIdentity();
		mCamera.applyMatrix();
		//glRotatef(totalTime * 50, 1.f, 0.f, 0.f);
		float x = (*chunk)->getX() * CHUNK_WIDTH;
		float y = (*chunk)->getY() * CHUNK_HEIGHT;
		float z = (*chunk)->getZ() * CHUNK_WIDTH;
		glTranslatef(x,y,z);

		//Draw VBO
		/*glBindBufferARB(GL_ARRAY_BUFFER_ARB, chunkVbo);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0,0);

		if(mRenderMode == RENDER_WIRE)
			glDrawElements(GL_LINES, chunkSize, GL_UNSIGNED_BYTE, 0);
		else
			glDrawElements(GL_QUADS, chunkSize, GL_UNSIGNED_BYTE, 0);

		glDisableClientState(GL_VERTEX_ARRAY);

		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);*/

		if( (*chunk)->hasGenerated() ) {

			GLgeometry* chunkGeom = (*chunk)->getGeometry();

			glVertexPointer(3, GL_FLOAT, sizeof(GLvertex), &(chunkGeom->vertexData[0].x));
			glNormalPointer(GL_FLOAT, sizeof(GLvertex), &(chunkGeom->vertexData[0].nx));
			glTexCoordPointer(2, GL_FLOAT, sizeof(GLvertex), &(chunkGeom->vertexData[0].u0));
			glClientActiveTexture(GL_TEXTURE0);

			// Draw the chunk.
			glDrawRangeElements(GL_TRIANGLES, 0, chunkGeom->vertexCount, chunkGeom->edgeCount, GL_UNSIGNED_SHORT, chunkGeom->edgeData);
		}
	}

	if( tex != 0 )
	{
		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glVertexPointer(3, GL_FLOAT, 0, 0);
	glNormalPointer(GL_FLOAT, 0, 0);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_CULL_FACE);

	drawStats( dt, chunks.size() );
}

void Renderer::drawStats(double dt, size_t chunkCount)
{
	// Switch to 2D for overlays
	enable2D();
	/*size_t memCurrent,memPeak,pagedCurrent,pagedPeak,pageFaults;
	Util::getMemoryUsage(memCurrent,memPeak,pagedCurrent,pagedPeak,pageFaults);*/

	char buff[500];
	size_t percent = ( mBlTotal > 0 ? (mBlRendered*100)/(mBlTotal) : 0 );
	sprintf( buff,  "Opencraft Performance:\n dt: %f\n %f FPS\n Avg: %u\n"
					"World Stats:\n Blocks: %u/%u - %u%%\n Rendered Chunks: %u\n"
					"Camera:\n Position: %f %f %f",
					dt, (1/dt), mFpsAvg, mBlRendered, mBlTotal, percent, chunkCount,
					mCamera.getPosition().x,mCamera.getPosition().y,mCamera.getPosition().z);
	std::string stats(buff);

	drawText( stats, 6, 15 );

	disable2D();
}

void Renderer::drawText(std::string text, int x, int y)
{
	stringvector lines = Util::split(text, '\n');
	glColor3f(0,1,0);
	size_t line = 0;
	void* fnt = GLUT_BITMAP_9_BY_15;
	for(stringvector::iterator it = lines.begin(); it != lines.end(); ++it) {
		glRasterPos2i( x, y + (line * 16) );
		for( std::string::iterator i = (*it).begin(); i != (*it).end(); ++i )
		{
			glutBitmapCharacter(fnt, *i);
		}
		line++;
	}
}
