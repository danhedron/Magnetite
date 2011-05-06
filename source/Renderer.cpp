#include "Renderer.h"
#include "WorldChunk.h"
#include "BaseBlock.h"

Renderer::Renderer(void)
: totalTime( 0 ),
chunkVbo( 0 )
{
}

Renderer::~Renderer(void)
{
	if( chunkVbo != 0 )
		glDeleteBuffersARB(1, &chunkVbo);
}

void Renderer::initialize()
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
}

void Renderer::resizeViewport(size_t x, size_t y, size_t w, size_t h)
{
	glViewport(x, y, w, h);
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

static GLfloat* PositionData = NULL;
static GLuint PositionSize;

void Renderer::buildChunkVBO(WorldChunk* chunk)
{
	PositionSize = chunk->getBlockCount() * 6 * 4 * 3 * sizeof(GLfloat);
	PositionData = new GLfloat[chunk->getBlockCount() * 6 * 4 * 3];
	chunkSize = chunk->getBlockCount() * 6 * 4;
	
	BlockList* blocks = chunk->getBlocks();
	size_t ind = 0;
	//glTranslatef( (*block)->getX(), (*block)->getY(), (*block)->getZ() );
	for(BlockList::iterator block = blocks->begin(); block != blocks->end(); ++block) {
		buildCubeData((*block).second, ind, PositionData);
	}

	//glGenBuffersARB(1, &chunkVbo);
	//glBindBufferARB(GL_ARRAY_BUFFER_ARB, chunkVbo);
	
	//glBufferDataARB(GL_ARRAY_BUFFER_ARB, chunkSize, PositionData, GL_STATIC_DRAW_ARB);
	
	//delete[] PositionData;
}

void Renderer::render(double dt, std::vector<WorldChunk*> &chunks)
{
	totalTime += dt;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	for(std::vector<WorldChunk*>::iterator chunk = chunks.begin();
		chunk != chunks.end();
		++chunk)
	{
		if(PositionData == NULL) {
			//Generate this chunk's vertex buffer object
			// Get blocks in this chunk.
			BlockList* blocks = (*chunk)->getBlocks();
			buildChunkVBO( (*chunk) );
		}
		
		// Sort out view Matrix.
		glLoadIdentity();
		glTranslatef(0.f, -10.f, -50.f);
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

		glDrawElements(GL_QUADS, chunkSize, GL_UNSIGNED_BYTE, 0);

		glDisableClientState(GL_VERTEX_ARRAY);

		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);*/
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, PositionData);
		// Draw the chunk.
		glDrawArrays(GL_QUADS, 0, chunkSize);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
}