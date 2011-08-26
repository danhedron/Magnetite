#include "Renderer.h"
#include "WorldChunk.h"
#include "World.h"
#include "Sky.h"
#include "BaseBlock.h"
#include "MagnetiteCore.h"
#include "TextureManager.h"
#include "Camera.h"
#include "BlockFactory.h"
#include "util.h"

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

// Shader paremater indexes.
size_t attrTC = 0; 
size_t attrL = 0;

void GLgeometry::releaseBuffer()
{
	if( this->vertexBO != 0 ) {
		glDeleteBuffers( 1, &this->vertexBO );
		this->vertexBO = 0;
	}
	if( this->indexBO != 0 ) {
		glDeleteBuffers( 1, &this->indexBO );
		this->indexBO = 0;
	}
}

void GLgeometry::bindToBuffer()
{
	glGenBuffers(1, &this->vertexBO);
	glBindBuffer( GL_ARRAY_BUFFER, this->vertexBO);
	glBufferData( GL_ARRAY_BUFFER, sizeof(GLvertex)*this->vertexCount+1, this->vertexData, GL_STATIC_DRAW );
	glGenBuffers(1, &this->indexBO);

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, this->indexBO );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(GLedge)*this->edgeCount+1, this->edgeData, GL_STATIC_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0);
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GLshader::create()
{
	if( ref != 0 || source.length() == 0 )
		return;

	ref = glCreateShader( type );
	if( ref == 0 )
		return;

	const char* src = source.c_str();
	glShaderSource( ref, 1, &src, NULL );
	glCompileShader( ref );
	
	GLint success;
	glGetObjectParameterivARB( ref, GL_COMPILE_STATUS, &success );
	if( success ) {
		Util::log("Shader Compiled OK: " + Util::toString(ref));
	}
	else {
		Util::log("Error compiling shader: ");
	}

	GLint blen = 0;	
	GLsizei slen = 0;

	glGetShaderiv(ref, GL_INFO_LOG_LENGTH , &blen);       
	if (blen > 1)
	{
		GLchar* compiler_log = (GLchar*)malloc(blen);
		glGetInfoLogARB(ref, blen, &slen, compiler_log);
		Util::log("\t" + std::string(compiler_log));
		free (compiler_log);
	}
}

void GLprogram::link()
{
	if( ref != 0 ) {
		Util::log("Program already linked");
		return;
	}
	if( vertex == NULL ) {
		Util::log("Vertex Shader missing");
		return;
	}
	if( fragment == NULL ) {
		Util::log("Fragment Shader missing");
		return;
	}
	if( vertex->ref == 0 )
	{
		// Init vertex shader.
		vertex->create();
		if( vertex->ref == 0 ) {
			//return;
		}
	}
	if( fragment->ref == 0 )
	{
		// Init fragment shader.
		fragment->create();
		if( fragment->ref == 0 ) {
			//return;
		}
	}
	if( geometry != NULL ) {
		if( geometry->ref == 0 ) {
			geometry->create();
			//return;
		}
	}

	ref = glCreateProgram();

	glAttachShader( ref, vertex->ref );
	if( geometry != NULL ) {
		glAttachShader( ref, geometry->ref );
	}
	glAttachShader( ref, fragment->ref );


	// deal with attributes n stuff
	for( auto it = mAttributes.begin(); it != mAttributes.end(); it++ )
	{
		glBindAttribLocation(ref, it->first, it->second.c_str());
	}

	glLinkProgram( ref );

	GLint linked;
	glGetProgramiv(ref, GL_LINK_STATUS, &linked);
	if (linked)
	{
	   Util::log("Program Linked OK");
	}
	else
	{
		Util::log("Error Linking Program:");
	}

	GLint blen = 0;	
	GLsizei slen = 0;

	glGetProgramiv(ref, GL_INFO_LOG_LENGTH , &blen);       
	if (blen > 1)
	{
		GLchar* compiler_log = (GLchar*)malloc(blen);
		glGetInfoLogARB(ref, blen, &slen, compiler_log);
		Util::log("\t" + std::string(compiler_log));
		free (compiler_log);
	}
}

void GLprogram::bindUniformTexture( std::string var, GLint unit )
{
	//if( mUniforms.find( var ) != mUniforms.end() ) return;
	const char* v = var.c_str();
	GLuint loc = glGetUniformLocation( ref, v );
	glUseProgram( ref );
	glUniform1i( loc, unit );
	mUniforms[var] = loc;
}

void GLprogram::bindAttribute( int index, std::string attribute )
{
	mAttributes[index] = attribute;
}

Renderer::Renderer(void)
: totalTime( 0 ),
chunkVbo( 0 ),
mScrWidth( 0 ),
mScrHeight( 0 ),
mBlRendered( 0 ),
mBlTotal( 0 ),
mRenderMode( RENDER_SOLID ),
mGeomType( GEOM_FALLBACK ),
mDebugMode( DEBUG_OFF ),
mFpsAvg( 0 ),
mCamera( NULL ),
blockType( "" ),
mDrawFrustum( false ),
mDrawWorld( true )
{
}

Renderer::~Renderer(void)
{
	if( chunkVbo != 0 )
		glDeleteBuffersARB(1, &chunkVbo);

	for( std::map<std::string, GLshader*>::iterator it = mShaders.begin(); it != mShaders.end(); it++ ) {
		glDeleteShader( it->second->ref );
		mShaders.erase( it++ );
	}
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
	
	MagnetiteCore::Singleton->getTextureManager()->loadTexture("../resources/ui/crosshair.png");

	// Test shader code.
	mWorldProgram.vertex = loadShader("w_vertex.glsl", GL_VERTEX_SHADER);
	mWorldProgram.fragment = loadShader("w_fragment.glsl", GL_FRAGMENT_SHADER);
	if( getGeometryType() == GEOM_GEOMSHADER ) {
		//mWorldProgram.geometry = loadShader("w_geometry.glsl", GL_GEOMETRY_SHADER_EXT);
	}
	mWorldProgram.link();

	// Find the shader parameters.
	attrTC = glGetAttribLocation( mWorldProgram.ref, "in_p" );
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

GLvertex Renderer::vertex(float x, float y, float z, float nx, float ny, float nz, float u, float v, float r, float g, float b)
{
	size_t u0 = u;
	size_t v0 = v;
	size_t l0 = r * 255;
	GLvertex vert = { x, y, z, u0, v0, l0 }; //nx, ny, nz,
	return vert;
}

void Renderer::buildCubeData(BaseBlock* block, size_t& ind, size_t& eInd, GLvertex* data, GLedge* edges)
{
	
}

GLshader* Renderer::loadShader( std::string filename, GLenum type )
{
	if( mShaders.find(filename) != mShaders.end() ) {
		return mShaders[filename];
	}

	std::string shaderPath = "../resources/shaders/";
	std::ifstream file( (shaderPath+filename).c_str() );
	if( !file ) return NULL;

	std::stringstream ss;
	std::string s;
	while( !file.eof() ) {
		std::getline( file, s );
		ss << s << '\n';
	}

	GLshader* shader = new GLshader();
	shader->source = ss.str();
	shader->filename = filename;
	shader->type = type;

	mShaders[filename] = shader;

	return shader;
}

void Renderer::unloadShader( std::string filename )
{
	if( mShaders.find(filename) != mShaders.end() ) {
		glDeleteShader( mShaders[filename]->ref );
		mShaders.erase( mShaders.find(filename) );
	}

}

static size_t rendered;
void Renderer::render(double dt, World* world)
{
	totalTime += dt;
	mFpsAvg = (mFpsAvg + (1/dt)) / 2;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1,1,1);

	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	rendered = 0;

	NodeList nodes = world->getTopNodes(); // world->getChunks();

	GLtexture* tex = MagnetiteCore::Singleton->getTextureManager()->fetchTexture("../resources/sprites/world.png");
	
	mCamera->applyMatrix( true, false );

	world->getSky()->renderSky();

	if( mRenderMode == RENDER_SOLID ) {
		GLint texLoc = glGetUniformLocation( mWorldProgram.ref, "worldDiffuse");
		glUseProgram( mWorldProgram.ref );
		glUniform1i( texLoc, 0 );
	}

	if( mDrawWorld ) 
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		//glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		if( tex != 0 )
		{
			glEnable(GL_TEXTURE_2D);
			glActiveTexture( GL_TEXTURE0 );
			glBindTexture(GL_TEXTURE_2D, tex->glID);
		}

		for(NodeList::iterator node = nodes.begin();
			node != nodes.end();
			++node)
		{
			_renderNode( *node, 0 );
		}

		if( tex != 0 )
		{
			glDisable(GL_TEXTURE_2D);
			glActiveTexture( GL_TEXTURE0 );
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		//glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisable(GL_CULL_FACE);
	}

	glUseProgram( 0 );

	if( mDrawFrustum )
	{
		glLoadIdentity();
		mCamera->applyMatrix(true, false);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(GLvertex), &(mCamera->getFrustum().mFrustumVolume->vertexData[0].x));

		glDrawRangeElements(GL_LINE_STRIP, 0, mCamera->getFrustum().mFrustumVolume->vertexCount, mCamera->getFrustum().mFrustumVolume->edgeCount, GL_UNSIGNED_SHORT, mCamera->getFrustum().mFrustumVolume->edgeData);
		glDisableClientState(GL_VERTEX_ARRAY);
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

	drawBlockChooser( dt );

	drawCrosshair( dt );
}

void Renderer::_renderNode(WorldNode *node, int depth)
{
	float nodeWorldSize = NODE_SIZE/pow(2.f, depth);

	Vector3 min = Vector3( node->index.x * CHUNK_WIDTH, node->index.y * CHUNK_HEIGHT, node->index.z * CHUNK_WIDTH );
	Vector3 max = Vector3( min.x + nodeWorldSize, min.y + CHUNK_HEIGHT, min.z + nodeWorldSize );

	size_t pos = Frustum::INSIDE;
	
	for( int i = 0; i < 4; i++ ) {
		WorldNode* child = node->children[i];
		if( child == NULL ) continue;
		if( child->isChunk && child->children[0] != NULL ) {
			_renderChunk( (WorldChunk*)child->children[0] ); // I am *So* sorry.
		}
		else
			_renderNode( child, depth + 1 );
	}

}

void Renderer::_renderChunk( WorldChunk* chunk )
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
	
	if( chunk->hasGenerated() ) {
		GLgeometry* chunkGeom = chunk->getGeometry();

		if( chunkGeom->vertexBO == 0 || chunkGeom->indexBO == 0 )
		{
			chunkGeom->bindToBuffer();
		}
		
		glBindBuffer( GL_ARRAY_BUFFER, chunkGeom->vertexBO );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, chunkGeom->indexBO );

		glVertexAttribPointer( attrTC, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(GLvertex), BUFFER_OFFSET(12) );
		//glVertexAttribPointer( attrL, 1, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(GLvertex), BUFFER_OFFSET(15) );
		//glEnableVertexAttribArray(attrL);
		glEnableVertexAttribArray(attrTC);


		glVertexPointer( 3, GL_FLOAT, sizeof(GLvertex), BUFFER_OFFSET(0) );
		//glNormalPointer( GL_FLOAT, sizeof(GLvertex), BUFFER_OFFSET(12) );
		//glTexCoordPointer( 2, GL_UNSIGNED_SHORT, sizeof(GLvertex), BUFFER_OFFSET(12));
		//glColorPointer( 1, GL_UNSIGNED_SHORT, sizeof(GLvertex), BUFFER_OFFSET(15));

		glDrawRangeElements( GL_TRIANGLES, 0, chunkGeom->vertexCount, chunkGeom->edgeCount, GL_UNSIGNED_SHORT, 0);
	
		glDisableVertexAttribArray(attrTC);
		//glDisableVertexAttribArray(attrL);
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	}
	else {
		Util::log("Warning: Ungenerated Chunk in render queue. ~ Grab a developer and complain");
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

void Renderer::drawBlockChooser( double dt )
{
	BlockFactoryList::iterator iter = FactoryManager::getManager().blockFactoryList.find(blockType);
	if( iter == FactoryManager::getManager().blockFactoryList.end() )
	{
		iter =FactoryManager::getManager().blockFactoryList.begin();
		blockType = iter->first;
	}
	
	std::stringstream ss;
	ss << "Current Block: " << iter->first << std::endl;
	drawText( ss.str(), 6, 200);
}

void Renderer::drawCrosshair( double dt )
{
	// Switch to 2D for overlays
	enable2D();

	GLtexture* tex = MagnetiteCore::Singleton->getTextureManager()->fetchTexture("../resources/ui/crosshair.png");
	
	if( tex != 0 )
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, tex->glID);
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

	if( tex != 0 )
	{
		glDisable(GL_TEXTURE_2D);
	}

	disable2D();
}

void Renderer::nextBlock()
{
	BlockFactoryList::iterator iter = FactoryManager::getManager().blockFactoryList.find(blockType);
	if( iter == FactoryManager::getManager().blockFactoryList.end() || ++iter == FactoryManager::getManager().blockFactoryList.end() )
	{
		iter = FactoryManager::getManager().blockFactoryList.begin();
	}

	blockType = iter->first;
}

void Renderer::lastBlock()
{
	BlockFactoryList::iterator iter = FactoryManager::getManager().blockFactoryList.find(blockType);
	if( iter == FactoryManager::getManager().blockFactoryList.begin() )
	{
		iter = FactoryManager::getManager().blockFactoryList.end();
	}
	--iter;


	blockType = iter->first;
}

void Renderer::drawText(std::string text, int x, int y)
{
	mWindow->PreserveOpenGLStates(true);
	sf::String drawableString(text, sf::Font::GetDefaultFont(), 14.5f);
	drawableString.SetColor(sf::Color(0,255,0));
	drawableString.SetPosition( x, y );
	mWindow->Draw(drawableString);
	mWindow->PreserveOpenGLStates(false);
}

