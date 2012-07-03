#include "Renderer.h"
#include "World.h"
#include "WorldTree.h"
#include "Sky.h"
#include "BaseBlock.h"
#include "MagnetiteCore.h"
#include "TextureManager.h"
#include "ProgramResource.h"
#include "Texture.h"
#include "ResourceManager.h"
#include "ShaderResource.h"
#include "Camera.h"
#include "util.h"

#include "glgeometry.h"

Renderer::Renderer(void)
: totalTime( 0 ),
mScrWidth( 0 ),
mScrHeight( 0 ),
mBlRendered( 0 ),
mBlTotal( 0 ),
mRenderMode( RENDER_SOLID ),
mDebugMode( DEBUG_OFF ),
mCamera( NULL ),
mFpsAvg( 0 ),
mDrawFrustum( false ),
mDrawWorld( true ),
mWorldProgram( NULL ),
mWorldTexture( NULL ),
mGeomType( GEOM_FALLBACK )
{
}

Renderer::~Renderer(void)
{
}

void Renderer::initialize(sf::RenderWindow& window)
{
	mWindow = &window;

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

	// Detect Geometry shader support
	/*if( !GL_EXT_geometry_shader4 ) {
		Util::log("=========== Warning ===========");
		Util::log("Your GPU doesn't support geometry shaders");
		Util::log("We will fall back to normal polygons for now");
		Util::log("but seriously it's 2011, you should upgrade");
		Util::log("===============================");
		mGeomType = GEOM_FALLBACK;
	}
	else
	{*/
		// k
		mGeomType = GEOM_GEOMSHADER;
	//}
	
	mCrosshair = MagnetiteCore::Singleton->getResourceManager()->getResource<Texture>("crosshair.png");
	mCrosshair->load();
	
	mWorldTexture = MagnetiteCore::Singleton->getResourceManager()->getResource<Texture>("world.png");
	mWorldTexture->load();

	mWorldProgram = MagnetiteCore::Singleton->getResourceManager()->getResource<ProgramResource>("world.prog");
	if( mWorldProgram == NULL ) {
		Util::log("World Program Missing!");
	}
	mWorldProgram->link();
	
	//attrL = glGetAttribLocation( mWorldProgram.ref, "in_light" );

	//mLightingProgram.vertex = loadShader("w_vertex.glsl", GL_VERTEX_SHADER);
	//mLightingProgram.fragment = loadShader("l_fragment.glsl", GL_FRAGMENT_SHADER);
	//mLightingProgram.link();
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
	if(mCamera) {
		mCamera->getFrustum().setAspectRatio(aspect);
		glMultMatrixf(mCamera->getFrustum().getPerspective().matrix);
	}
	else {
		gluPerspective(90.f, aspect, 0.1f, 500.f);
	}
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
	glPopMatrix();
}

void Renderer::setRenderMode( size_t rendermode )
{
	mRenderMode = rendermode;
}

size_t Renderer::getRenderMode()
{
	return mRenderMode;
}

GeomType Renderer::getGeometryType()
{
	return mGeomType;
}

void Renderer::setDebugMode( size_t debugMode )
{
	mDebugMode = debugMode;
}

Camera* Renderer::getCamera()
{
	return mCamera;
}

void Renderer::addDebugLine( const Vector3& start, const Vector3& end )
{
	mDebugLines.push_back( Line( start, end ) );
}

void Renderer::setWorldVisible( bool vis )
{
	mDrawWorld = vis;
}

bool Renderer::isWorldVisible()
{
	return mDrawWorld;
}

void Renderer::setCamera( Camera* cam )
{
	mCamera = cam;
}

void Renderer::toggleCameraFrustum()
{
	mDrawFrustum = !mDrawFrustum;
	if( mDrawFrustum ) {
		mCamera->getFrustum().updateFrustumVolume();
	}
}

static size_t rendered;
void Renderer::render(double dt, World* world)
{
	totalTime += dt;
	mFpsAvg = (mFpsAvg + (1/dt)) / 2;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1,1,1);
	

	if( mDebugMode == DEBUG_LINES ) {
		for( LineList::iterator it = mDebugLines.begin(); it != mDebugLines.end(); it++ ) 
		{
			Line l = *it;
			glBegin(GL_LINES);
			glColor3f(1, 1, 1);
			glVertex3d(l.first.x, l.first.y, l.first.z);
			glVertex3d(l.second.x, l.second.y, l.second.z);
			glEnd();

		}
	}

	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	mCamera->applyMatrix( true, false );
	if( world->getSky() != NULL )
	{
		world->getSky()->renderSky();
	}
	
	rendered = 0;

	ChunkArray chunks = world->getChunks();

	if( mDrawWorld && mDebugMode != DEBUG_SKY ) 
	{
		mWorldProgram->makeActive();
		
		// Set up the uniform for the world diffuse texture.
		GLint samplerLocation = mWorldProgram->getUniformLocation("worldDiffuse");
		if( samplerLocation != -1 )
		{
			glEnable(GL_TEXTURE_2D);
			glUniform1i( samplerLocation, 0 );
			glActiveTexture( GL_TEXTURE0 );
			glBindTexture(GL_TEXTURE_2D, mWorldTexture->getName());
		}
		
		
		for( size_t c = 0; c < world->getChunkCount(); c++ )
		{
			if( chunks[c] )
			{
				if( chunks[c]->getMutex().try_lock() )
				{
					_renderChunk( chunks[c] );
					chunks[c]->getMutex().unlock();
				}
			}
		}
		
		// Draw moving blocks
		MovingBlockList& moving = world->getMovingBlocks();
		for( MovingBlock& b : moving )
		{
			GLgeometry* geom = b.geom;
			if( geom->vertexBO == 0 || geom->indexBO == 0 )
			{
				geom->bindToBuffer();
				// Check everything went ok, if not skip the block.
				if( geom->vertexBO == 0 || geom->indexBO == 0 ) continue;
			}
			
			glBindBuffer( GL_ARRAY_BUFFER, geom->vertexBO );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, geom->indexBO );
			
			auto vertexA = mWorldProgram->getAttributeIndex("in_vertex");
			glEnableVertexAttribArray(vertexA);
			glVertexAttribPointer( vertexA, 3, GL_FLOAT, GL_FALSE, sizeof(GLvertex), BUFFER_OFFSET(0) );

			// The UV information in packed into one attribute, should split into 2.
			auto paramA = mWorldProgram->getAttributeIndex("in_params");
			glEnableVertexAttribArray(paramA);
			glVertexAttribPointer( paramA, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(GLvertex), BUFFER_OFFSET(12) );
			
			glDrawRangeElements( GL_TRIANGLES, 0, geom->vertexCount, geom->edgeCount, GL_UNSIGNED_SHORT, 0);

			glDisableVertexAttribArray(paramA);
			glDisableVertexAttribArray(vertexA);
		}
		
		if( samplerLocation != -1 )
		{
			glDisable(GL_TEXTURE_2D);
			glActiveTexture( GL_TEXTURE0 );
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		
		//Detatch the world program
		glUseProgram( 0 );
	}
	
	glLoadIdentity();
	mCamera->applyMatrix();

	switch( mDebugMode ) {
		case DEBUG_STATS:
			drawStats( dt, rendered, world );
			break;
		case DEBUG_PHYSICS:
			MagnetiteCore::Singleton->getPhysicsWorld()->debugDrawWorld();
			break;
	};

	drawCrosshair( dt );
}

void Renderer::_renderTree(WorldTree* tree)
{
}

void Renderer::_renderNode(WorldNode *node, int depth)
{
}

void Renderer::_renderChunk( Chunk* chunk )
{
	rendered++;
	// Sort out view Matrix.
	glLoadIdentity();
	mCamera->applyMatrix();
	//glRotatef(totalTime * 50, 1.f, 0.f, 0.f);
	float x = chunk->getX() * CHUNK_WIDTH;
	float y = chunk->getY() * CHUNK_HEIGHT;
	float z = chunk->getZ() * CHUNK_WIDTH;
	glTranslatef(x,y,z);
	
	if( !chunk->_hasChunkFlag( Chunk::MeshInvalid ) && chunk->getGeometry() != NULL ) {
		GLgeometry* geom = chunk->getGeometry();
		
		if( geom->vertexBO == 0 || geom->indexBO == 0 )
		{
			geom->bindToBuffer();
			if( geom->vertexBO == 0 || geom->indexBO == 0 ) return;
		}
        
		glBindBuffer( GL_ARRAY_BUFFER, geom->vertexBO );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, geom->indexBO );
		
		auto vertexA = mWorldProgram->getAttributeIndex("in_vertex");
		glEnableVertexAttribArray(vertexA);
		glVertexAttribPointer( vertexA, 3, GL_FLOAT, GL_FALSE, sizeof(GLvertex), BUFFER_OFFSET(0) );

		// The UV information in packed into one attribute, should split into 2.
		auto paramA = mWorldProgram->getAttributeIndex("in_params");
		glEnableVertexAttribArray(paramA);
		glVertexAttribPointer( paramA, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(GLvertex), BUFFER_OFFSET(12) );
		
		glDrawRangeElements( GL_TRIANGLES, 0, geom->vertexCount, geom->edgeCount, GL_UNSIGNED_SHORT, 0);

		glDisableVertexAttribArray(paramA);
		glDisableVertexAttribArray(vertexA);

		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	}

}

void Renderer::drawStats(double dt, size_t chunkCount, World* world)
{
	size_t percent = ( mBlTotal > 0 ? (mBlRendered*100)/(mBlTotal) : 0 );

	std::stringstream ss;
	ss << "Magnetite Info:" << std::endl;
	ss << "\tdt: " << dt << std::endl;
	ss << "\tFPS: " << (1.f/dt) << std::endl;
	ss << "\tTimescale: " << MagnetiteCore::Singleton->getTimescale() << std::endl;
	ss << "World Stats: " << std::endl;
	ss << "\tBlocks: " << mBlRendered << "/" << mBlTotal << " - " << percent <<  std::endl;
	ss << "\tRendered Chunks: " << chunkCount << std::endl;
	ss << "\tTime: " << world->getSky()->getTime() % DAY_LENGTH << std::endl;
	ss << "Camera: " << std::endl;
	ss << "\tPosition: " << Util::toString(mCamera->getPosition()) << std::endl;

	drawText( ss.str(), 6, 15 );
}

void Renderer::drawCrosshair( double dt )
{
	// Switch to 2D for overlays
	enable2D();

	if( mCrosshair != NULL ) 
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, mCrosshair->getName());
	}

	glEnable (GL_BLEND);

	glColor3f(1,1,1);

	glBegin(GL_QUADS);
	glTexCoord2f(0.f,0.f);
	glVertex2i( (mScrWidth/2) + 8, (mScrHeight/2) + 8);
	glTexCoord2f(0.f,1.f);
	glVertex2i( (mScrWidth/2) - 8, (mScrHeight/2) + 8);
	glTexCoord2f(1.f,1.f);
	glVertex2i( (mScrWidth/2) - 8, (mScrHeight/2) - 8);
	glTexCoord2f(1.f,0.f);
	glVertex2i( (mScrWidth/2) + 8, (mScrHeight/2) - 8);
	glEnd();

	glDisable (GL_BLEND);

	if( mCrosshair != NULL )
	{
		glDisable(GL_TEXTURE_2D);
	}

	disable2D();
}

void Renderer::drawText(std::string text, int x, int y)
{
	mWindow->pushGLStates();
	sf::Text drawableString(text, sf::Font::getDefaultFont(), 17.f);
	drawableString.setColor(sf::Color(0,255,0));
	drawableString.setPosition( x, y );
	mWindow->draw(drawableString);
	mWindow->popGLStates();
}

