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
#include <glm/gtc/type_ptr.hpp>

#include "Geometry.h"
#include <Component.h>
#include <BaseEntity.h>
#include <Profiler.h>

MeshGeometry* crosshairGeom = NULL;

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
	
	if( crosshairGeom == NULL ) 
	{
		crosshairGeom = new MeshGeometry();
		crosshairGeom->vertexCount = 4;
		crosshairGeom->edgeCount = 6;
		
		crosshairGeom->vertexData = new GeometryVertex[crosshairGeom->vertexCount];
		crosshairGeom->edgeData = new GLedge[crosshairGeom->vertexCount];
		
		size_t i = 0;
		size_t hw = mWindow->getSize().x/2;
		size_t hh = mWindow->getSize().y/2;
		crosshairGeom->vertexData[i++] = Geometry::vertex( hw + 8, hh + 8, 0 );
		crosshairGeom->vertexData[i++] = Geometry::vertex( hw - 8, hh + 8, 0 );
		crosshairGeom->vertexData[i++] = Geometry::vertex( hw - 8, hh - 8, 0 );
		crosshairGeom->vertexData[i++] = Geometry::vertex( hw + 8, hh - 8, 0 );
		
		/*glTexCoord2f(0.f,0.f);
		glVertex2i( (mScrWidth/2) + 8, (mScrHeight/2) + 8);
		glTexCoord2f(0.f,1.f);
		glVertex2i( (mScrWidth/2) - 8, (mScrHeight/2) + 8);
		glTexCoord2f(1.f,1.f);
		glVertex2i( (mScrWidth/2) - 8, (mScrHeight/2) - 8);
		glTexCoord2f(1.f,0.f);
		glVertex2i( (mScrWidth/2) + 8, (mScrHeight/2) - 8);*/

	}
	
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
		glMultMatrixf( glm::value_ptr( mCamera->getFrustum().getPerspective()  ) );
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
	
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glLoadIdentity();
	glMultMatrixf( glm::value_ptr( glm::mat4( glm::inverse(mCamera->getOrientationMatrix() ) ) ) );
	if( world->getSky() != NULL )
	{
		glDisable( GL_DEPTH_TEST );
		world->getSky()->renderSky();
		glEnable( GL_DEPTH_TEST );
	}
	
	rendered = 0;

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
		
		// Just draw everything, need some occulusion technique.
		auto regions = world->getRegions();
		for( size_t r = 0;  r < world->getRegionCount(); r++ )
		{
			if( regions[r] == NULL ) continue;
			for( size_t c = 0; c < regions[r]->count(); c++ )
			{
				auto chnk = regions[r]->get(c);
				if( chnk )
				{
					_renderChunk( regions[r], chnk );
				}
			}
		}
		
		// Draw moving blocks
		MovingBlockList& moving = world->getMovingBlocks();
		for( MovingBlock& b : moving )
		{
			TerrainGeometry* geom = b.geom;
			if( geom->vertexBO == 0 || geom->indexBO == 0 )
			{
				geom->bindToBuffer();
				// Check everything went ok, if not skip the block.
				if( geom->vertexBO == 0 || geom->indexBO == 0 ) continue;
			}
			
			glBindBuffer( GL_ARRAY_BUFFER, geom->vertexBO );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, geom->indexBO );
		
			geom->bindVertexAttributes(mWorldProgram);
			
			glDrawRangeElements( GL_TRIANGLES, 0, geom->vertexCount, geom->edgeCount, GL_UNSIGNED_SHORT, 0);
			
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}
		
		if( samplerLocation != -1 )
		{
			glDisable(GL_TEXTURE_2D);
			glActiveTexture( GL_TEXTURE0 );
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		
		//Detatch the world program
		mWorldProgram->deactivate();
	}
	
	auto entities = world->getEntities();
	Magnetite::Component::DrawInfo i;
	i.projection = mCamera->getFrustum().getPerspective();
	i.view = glm::inverse(mCamera->getMatrix());
	for( auto it = entities.begin(); it != entities.end(); ++it )
	{
		(*it)->draw(i, dt);
	}
	

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

void Renderer::_renderChunk( Magnetite::ChunkRegionPtr region, Chunk* chunk )
{
	if( !chunk->_hasChunkFlag( Chunk::MeshInvalid ) && chunk->getGeometry() != NULL && chunk->getVisibleFaceCount() > 0 )
	{
		chunk->getMutex().lock();
		rendered++;
		// Sort out view Matrix.
		glLoadIdentity();
		auto view = glm::inverse( mCamera->getMatrix() );
		glMultMatrixf( glm::value_ptr(view) );
		
		float x = chunk->getX() * CHUNK_WIDTH;
		float y = chunk->getY() * CHUNK_HEIGHT;
		float z = chunk->getZ() * CHUNK_WIDTH;
		glTranslatef(x,y,z);
		
		Geometry* geom = chunk->getGeometry();
		
		if( geom->vertexBO == 0 || geom->indexBO == 0 )
		{
			geom->bindToBuffer();
			if( geom->vertexBO == 0 || geom->indexBO == 0 ) return;
		}
        
		glBindBuffer( GL_ARRAY_BUFFER, geom->vertexBO );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, geom->indexBO );
		
		geom->bindVertexAttributes(mWorldProgram);
		
		glDrawRangeElements( GL_TRIANGLES, 0, geom->vertexCount, geom->edgeCount, GL_UNSIGNED_SHORT, 0);

		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		
		chunk->getMutex().unlock();
	}

}

void Renderer::drawStats(double dt, size_t chunkCount, World* world)
{
	size_t percent = ( mBlTotal > 0 ? (mBlRendered*100)/(mBlTotal) : 0 );
	Perf::Profiler& perf = Perf::Profiler::get();
	
	std::stringstream ss;
	ss << "Magnetite Info:" << std::endl;
	ss << "\tdt: " << dt << std::endl;
	ss << "\tFPS: " << (1.f/dt) << std::endl;
	ss << "\tthink: " << perf.getLastTime( "think" ) << "ms" << std::endl;
	ss << "\tgame think: " << perf.getLastTime( "gthink" ) << "ms" << std::endl;
	ss << "\tdraw: " << perf.getLastTime( "draw" ) << "ms" << std::endl;
	ss << "\tTimescale: " << MagnetiteCore::Singleton->getTimescale() << std::endl;
	ss << "World Stats: " << std::endl;
	ss << "\tBlocks: " << mBlRendered << "/" << mBlTotal << " - " << percent <<  std::endl;
	ss << "\tRendered Chunks: " << chunkCount << std::endl;
	ss << "\tEntity Count: " << world->getEntities().size() << std::endl;
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

