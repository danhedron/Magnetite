#include "Renderer.h"
#include "WorldChunk.h"
#include "BaseBlock.h"

#include "util.h"
#include <stdio.h>

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
	gluPerspective(90.f, 1.f, 1.f, 500.f);

	glutInit( argc, argv );
}

void Renderer::resizeViewport(size_t x, size_t y, size_t w, size_t h)
{
	glViewport(x, y, w, h);
	mScrWidth = w;
	mScrHeight = h;
	std::stringstream msg;
	msg << "Window Resized: " << w << "x" << h;
	Util::log( msg.str() );
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

void Renderer::buildCubeData(BaseBlock* block, size_t& ind, GLfloat* data)
{
	/* Sorry */
	data[ind + 0] = block->getX() + 0.5f;
	data[ind + 1] = block->getY() + 0.5f;
	data[ind + 2] = block->getZ() + 0.5f;
	data[ind + 3] = block->getX() - 0.5f;
	data[ind + 4] = block->getY() + 0.5f;
	data[ind + 5] = block->getZ() + 0.5f;
	data[ind + 6] = block->getX() - 0.5f;
	data[ind + 7] = block->getY() - 0.5f;
	data[ind + 8] = block->getZ() + 0.5f;
	data[ind + 9] = block->getX() + 0.5f;
	data[ind + 10] = block->getY() - 0.5f;
	data[ind + 11] = block->getZ() + 0.5f;
	ind += 12;
	data[ind + 0] = block->getX() + 0.5f;
	data[ind + 1] = block->getY() + 0.5f;
	data[ind + 2] = block->getZ() - 0.5f;
	data[ind + 3] = block->getX() - 0.5f;
	data[ind + 4] = block->getY() + 0.5f;
	data[ind + 5] = block->getZ() - 0.5f;
	data[ind + 6] = block->getX() - 0.5f;
	data[ind + 7] = block->getY() - 0.5f;
	data[ind + 8] = block->getZ() - 0.5f;
	data[ind + 9] = block->getX() + 0.5f;
	data[ind + 10] = block->getY() - 0.5f;
	data[ind + 11] = block->getZ() - 0.5f;
	ind += 12;
	data[ind + 0] = block->getX() + 0.5f;
	data[ind + 1] = block->getY() + 0.5f;
	data[ind + 2] = block->getZ() + 0.5f;
	data[ind + 3] = block->getX() + 0.5f;
	data[ind + 4] = block->getY() - 0.5f;
	data[ind + 5] = block->getZ() + 0.5f;
	data[ind + 6] = block->getX() + 0.5f;
	data[ind + 7] = block->getY() - 0.5f;
	data[ind + 8] = block->getZ() - 0.5f;
	data[ind + 9] = block->getX() + 0.5f;
	data[ind + 10] = block->getY() + 0.5f;
	data[ind + 11] = block->getZ() - 0.5f;
	ind += 12;
	data[ind + 0] = block->getX() - 0.5f;
	data[ind + 1] = block->getY() - 0.5f;
	data[ind + 2] = block->getZ() + 0.5f;
	data[ind + 3] = block->getX() - 0.5f;
	data[ind + 4] = block->getY() - 0.5f;
	data[ind + 5] = block->getZ() - 0.5f;
	data[ind + 6] = block->getX() + 0.5f;
	data[ind + 7] = block->getY() - 0.5f;
	data[ind + 8] = block->getZ() - 0.5f;
	data[ind + 9] = block->getX() + 0.5f;
	data[ind + 10] = block->getY() - 0.5f;
	data[ind + 11] = block->getZ() + 0.5f;
	ind += 12;
	data[ind + 0] = block->getX() - 0.5f;
	data[ind + 1] = block->getY() + 0.5f;
	data[ind + 2] = block->getZ() + 0.5f;
	data[ind + 3] = block->getX() + 0.5f;
	data[ind + 4] = block->getY() + 0.5f;
	data[ind + 5] = block->getZ() + 0.5f;
	data[ind + 6] = block->getX() + 0.5f;
	data[ind + 7] = block->getY() + 0.5f;
	data[ind + 8] = block->getZ() - 0.5f;
	data[ind + 9] = block->getX() - 0.5f;
	data[ind + 10] = block->getY() + 0.5f;
	data[ind + 11] = block->getZ() - 0.5f;
	ind += 12;
	data[ind + 0] = block->getX() - 0.5f;
	data[ind + 1] = block->getY() + 0.5f;
	data[ind + 2] = block->getZ() - 0.5f;
	data[ind + 3] = block->getX() - 0.5f;
	data[ind + 4] = block->getY() - 0.5f;
	data[ind + 5] = block->getZ() - 0.5f;
	data[ind + 6] = block->getX() - 0.5f;
	data[ind + 7] = block->getY() - 0.5f;
	data[ind + 8] = block->getZ() + 0.5f;
	data[ind + 9] = block->getX() - 0.5f;
	data[ind + 10] = block->getY() + 0.5f;
	data[ind + 11] = block->getZ() + 0.5f;
	ind += 12;
}


static const int BufferSize = 1;
static GLuint BufferName[BufferSize];

enum {
	POSITION_OBJECT = 0
};

static GLuint PositionSize;

void Renderer::buildChunkVBO(WorldChunk* chunk)
{
	PositionSize = chunk->getBlockCount() * 6 * 4 * 3 * sizeof(GLfloat);
	GLfloat* PositionData = new GLfloat[chunk->getVisibleBlockCount() * 6 * 4 * 3];
	chunkSize = chunk->getVisibleBlockCount() * 6 * 4;

	BlockList* blocks = chunk->getVisibleBlocks();
	size_t ind = 0;
	//glTranslatef( (*block)->getX(), (*block)->getY(), (*block)->getZ() );
	for(BlockList::iterator block = blocks->begin(); block != blocks->end(); ++block) {
		buildCubeData((*block).second, ind, PositionData);
	}

	// Chunk has been defined, store it's data
	mWorldBuffers.insert( std::pair<WorldChunk*,GLfloat*>( chunk, PositionData ) );

	mBlRendered += chunk->getVisibleBlockCount();
	mBlTotal += chunk->getBlockCount();

	/*glGenBuffersARB(1, &chunkVbo);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, chunkVbo);

	glBufferDataARB(GL_ARRAY_BUFFER_ARB, PositionSize, PositionData, GL_STATIC_DRAW_ARB);*/

	//delete[] PositionData;
}

void Renderer::notifyChunkUnloaded( WorldChunk* chunk )
{
	ChunkGeomList::iterator it = mWorldBuffers.find( chunk );
	if(it != mWorldBuffers.end()) {
		// We have a buffer for this chunk, free it.
		delete[] it->second;
		mWorldBuffers.erase( it );
	}
}

void Renderer::render(double dt, std::vector<WorldChunk*> &chunks)
{
	totalTime += dt;
	mFpsAvg = (mFpsAvg + (1/dt)) / 2;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	for(std::vector<WorldChunk*>::iterator chunk = chunks.begin();
		chunk != chunks.end();
		++chunk)
	{
		ChunkGeomList::iterator it = mWorldBuffers.find( (*chunk) );

		if(it == mWorldBuffers.end()) {
			// This chunk hasn't been generated yet, fix that:
			buildChunkVBO( (*chunk) );
		}

		// Sort out view Matrix.
		glLoadIdentity();
		glTranslatef(0.f, -15.f, -50.f);
		//glRotatef(totalTime * 50, 1.f, 0.f, 0.f);
		glRotatef(totalTime * 10, 0.f, 1.f, 0.f);
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

		if( it != mWorldBuffers.end() )
		{
			GLfloat* PositionData = (*it).second;

			glColor3f(1,1,1);
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, 0, PositionData);
			// Draw the chunk.
			if(mRenderMode == RENDER_WIRE)
				glDrawArrays(GL_LINES, 0, chunkSize);
			else
				glDrawArrays(GL_QUADS, 0, chunkSize);
			glDisableClientState(GL_VERTEX_ARRAY);
		}
	}

	drawStats( dt, chunks.size() );
}

void Renderer::drawStats(double dt, size_t chunkCount)
{
	// Switch to 2D for overlays
	enable2D();
	char buff[200];
	long percent = ( mBlTotal > 0 ? (mBlRendered*100)/(mBlTotal) : 0 );
	sprintf( buff, "Opencraft Performance:\n dt: %f\n %f FPS\n Avg: %u\n"
		"World Stats:\n Blocks: %u/%u - %u%%\n Rendered Chunks: %u", dt, (1/dt), mFpsAvg, mBlRendered, mBlTotal, percent, chunkCount);
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
