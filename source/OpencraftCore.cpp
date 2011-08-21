#include "OpencraftCore.h"
#include "WorldChunk.h"
#include "Renderer.h"
#include "TextureManager.h"
#include "InputManager.h"
#include "StoneBlock.h"
#include "World.h"
#include "Character.h"
#include "BlockFactory.h"
#include "BaseGame.h"
#include "BulletDebug.h"
#include <ctime>

OpencraftCore* OpencraftCore::Singleton = 0;
static GLDebugDrawer debug;


/* Simple events */
void globalEventHandler( const InputEvent& evt )
{
	OpencraftCore::Singleton->getGame()->_inputEvents( evt );
	if( evt.event == Inputs::SCREENSHOT && evt.down == true ) {
		OpencraftCore::Singleton->screenshot();
	}
}

OpencraftCore::OpencraftCore(void)
: mContinue( true ),
mWorld( NULL ),
mTextureManager( NULL ),
mInputManager( NULL ),
mTimescale( 1.f ),
mLastX( 0.f ),
mLastY( 0.f ),
mPBroadphase( NULL ),
mPCConfig( NULL ),
mCCDispatch ( NULL ),
mSolver( NULL ),
mPhysicsWorld( NULL )
{
	OpencraftCore::Singleton = this;
	mRenderer = new Renderer();
	mTextureManager = new TextureManager();
	mInputManager = new InputManager();
	mInputManager->setEventCallback( Inputs::FORWARD, &globalEventHandler );
	mInputManager->setEventCallback( Inputs::BACK, &globalEventHandler );
	mInputManager->setEventCallback( Inputs::LEFT, &globalEventHandler );
	mInputManager->setEventCallback( Inputs::RIGHT, &globalEventHandler );
	mInputManager->setEventCallback( Inputs::JUMP, &globalEventHandler );
	mInputManager->setEventCallback( Inputs::SPRINT, &globalEventHandler );
	mInputManager->setEventCallback( Inputs::SCREENSHOT, &globalEventHandler );
	mInputManager->setEventCallback( Inputs::FLY, &globalEventHandler );
}

OpencraftCore::~OpencraftCore(void)
{
	unloadWorld();

	delete mRenderer;
	delete mTextureManager;
	delete mInputManager;

	mPhysicsWorld->removeRigidBody( mGroundBody );

	delete mGroundBody;
	delete mGroundShape;
	delete mGroundState;

	delete mPhysicsWorld;
	delete mSolver;
	delete mCCDispatch;
	delete mPCConfig;
	delete mPBroadphase;
}

void OpencraftCore::init(int *argc, char **argv)
{
	sf::WindowSettings wnds;
	wnds.DepthBits = 24;
	wnds.DepthBits = 8;
	//wnds.AntialiasingLevel = 2;
	mWindow.Create(sf::VideoMode(800,600,32), "Opencraft", sf::Style::Close | sf::Style::Resize, wnds);
	mWindow.EnableKeyRepeat( false );
	//mWindow.UseVerticalSync(true);
	mRenderer->initialize(mWindow);
	wglSwapIntervalEXT(1); // Enable VSYNC, for greater justice
	mTextureManager->initalize();
	mRenderer->resizeViewport(0,0,800,600);
	initalizePhysics();
}

void OpencraftCore::initalizePhysics()
{
	mPBroadphase = new btDbvtBroadphase();
	mPCConfig = new btDefaultCollisionConfiguration();
	mCCDispatch = new btCollisionDispatcher(mPCConfig);
	mSolver = new btSequentialImpulseConstraintSolver();
	mPhysicsWorld = new btDiscreteDynamicsWorld( mCCDispatch, mPBroadphase, mSolver, mPCConfig );
	mPhysicsWorld->setGravity( btVector3( 0.f, -9.81f, 0.f ) );

	mGroundShape = new btStaticPlaneShape( btVector3(0, 1, 0), 0 );
	mGroundState = new btDefaultMotionState;
	btRigidBody::btRigidBodyConstructionInfo ci( 0, mGroundState, mGroundShape, btVector3(0,0,0) );
	mGroundBody = new btRigidBody( ci );
	mPhysicsWorld->addRigidBody( mGroundBody );
	debug.setDebugMode( btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawAabb );
	mPhysicsWorld->setDebugDrawer( &debug );
}

btDiscreteDynamicsWorld* OpencraftCore::getPhysicsWorld()
{
	return mPhysicsWorld;
}

void OpencraftCore::screenshot()
{
	sf::Image screen = mWindow.Capture();
	time_t rawt = time( NULL );
	tm* t = localtime( &rawt );
	char tstr[80];
	strftime(tstr, 80, "%Y-%m-%d_%H-%M-%S", t);
	std::string fname("opencraft_");
	fname.append( tstr );
	fname.append(".png");
	Util::log("Saving screenshot to: " + fname);
	screen.SaveToFile(fname);
}

void OpencraftCore::startGame( const std::string& type )
{
	mGame = FactoryManager::getManager().createGame(type);
	if( mGame != NULL ) 
		Util::log("Starting game: " + mGame->getName() );
	mGame->_startGameSingle();
	
	newWorld("test");
	
	// No multiplayer yet so just force player join
	mGame->_playerJoined();
}

void OpencraftCore::inputMovement( const Vector3 &v )
{
	mGame->_inputMovement( v );
}

void OpencraftCore::mouseMoved( const float x, const float y )
{
	mGame->_mouseMoved( x, y );
}

void OpencraftCore::go() 
{
	// Print factory list for testing
	FactoryManager::getManager().logFactories();
	
	int lastX = mWindow.GetWidth()/2;
	int lastY = mWindow.GetHeight()/2;

	mClock.Reset();
	while(mContinue && mWindow.IsOpened()) {
		float lDelta = mClock.GetElapsedTime();
		mClock.Reset();

		// Handle Events before we render.
		sf::Event lEvt;
		while( mWindow.GetEvent(lEvt) ) {
			if( lEvt.Type == sf::Event::Closed ) {
				exit();
			}
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::Escape) ) {
				exit();
			}

			if( lEvt.Type == sf::Event::KeyPressed )
				mInputManager->keyDown( lEvt.Key.Code );
			if( lEvt.Type == sf::Event::KeyReleased )
				mInputManager->keyUp( lEvt.Key.Code );

			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::R) ) {
					if(mRenderer->getRenderMode() == Renderer::RENDER_SOLID)
						mRenderer->setRenderMode(Renderer::RENDER_LIGHTING);
					else
						mRenderer->setRenderMode(Renderer::RENDER_SOLID);
			}

			//==========
			//= Debug Buttons
			//==========
			if( (lEvt.Type == sf::Event::KeyReleased ) &&
				(lEvt.Key.Code == sf::Key::F1 ) ) {
				mRenderer->setDebugMode( Renderer::DEBUG_OFF );
			}
			if( (lEvt.Type == sf::Event::KeyReleased ) &&
				(lEvt.Key.Code == sf::Key::F3 ) ) {
				mRenderer->setDebugMode( Renderer::DEBUG_STATS );
			}
			if( (lEvt.Type == sf::Event::KeyReleased ) &&
				(lEvt.Key.Code == sf::Key::F4 ) ) {
				mRenderer->setDebugMode( Renderer::DEBUG_PHYSICS );
			}
			if( (lEvt.Type == sf::Event::KeyReleased ) &&
				(lEvt.Key.Code == sf::Key::F5 ) ) {
				Util::setLogLevel( Util::Verbose );
			}
			if( (lEvt.Type == sf::Event::KeyReleased) &&
				(lEvt.Key.Code == sf::Key::F) ) {
					//mPlayer->enableFlying( !mPlayer->isFlying() );
					//Util::log( (Util::toString(mPlayer->isFlying())) + " Flying");
			}
			if( (lEvt.Type == sf::Event::KeyReleased) &&
				(lEvt.Key.Code == sf::Key::F) ) {
					//mPlayer->enableFlying( !mPlayer->isFlying() );
					//Util::log( (Util::toString(mPlayer->isFlying())) + " Flying");
			}
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::Add) ) {
					mRenderer->nextBlock();
			}
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::F6) ) {
					mWorld->saveAllChunks();
			}
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::F7) ) {
					mWorld->loadWorld("test");
			}
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::Num9) ) {
					mTimescale *= 0.5f;
			}
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::Num0) ) {
					mTimescale *= 2.f;
			}
			if( (lEvt.Type == sf::Event::MouseMoved) ) {
					mGame->_mouseMoved( -(lEvt.MouseMove.X - lastX), -(lEvt.MouseMove.Y  - lastY));
					lastX = mWindow.GetWidth()/2;
					lastY = mWindow.GetHeight()/2;
					mWindow.SetCursorPosition( mWindow.GetWidth() / 2, mWindow.GetHeight() / 2 );
			}
			if( lEvt.Type == sf::Event::MouseButtonPressed && lEvt.MouseButton.Button == sf::Mouse::Left ) {
				mGame->_primary();
			}
			if( lEvt.Type == sf::Event::MouseButtonPressed && lEvt.MouseButton.Button == sf::Mouse::Right ) {
				mGame->_secondary();
			}
			if( lEvt.Type == sf::Event::Resized ) {
				mRenderer->resizeViewport( 0, 0, lEvt.Size.Width, lEvt.Size.Height );
			}
		}

		Util::log("Beginning Step");

		lDelta *= mTimescale;
		
		mPhysicsWorld->stepSimulation( lDelta );
		
		//Ensure each loaded chunk is updated before being sent to the GPU
		mWorld->update( lDelta );

		// Update all the characters
		for( std::vector<Character*>::iterator it = mCharacters.begin(); it != mCharacters.end(); it++ )
		{
			(*it)->update( lDelta );
		}
	
		//Ensure the renderer has the correct camera
		mRenderer->setCamera( mGame->getLocalPlayer()->getCamera() );
		mRenderer->render(lDelta, mWorld);

		mWindow.Display();
	}

	mWindow.Close();

}

void OpencraftCore::exit()
{
	mContinue = false;
}

TextureManager* OpencraftCore::getTextureManager()
{
	return mTextureManager;
}

Renderer* OpencraftCore::getRenderer()
{
	return mRenderer;
}

BaseGame* OpencraftCore::getGame()
{
	return mGame;
}

float OpencraftCore::getTimescale()
{
	return mTimescale;
}

void OpencraftCore::newWorld( std::string name )
{
	unloadWorld();

	mWorld = new World();
	mWorld->newWorld( name );
}

void OpencraftCore::unloadWorld()
{
	if( mWorld != NULL ) {
		// world handles saving.
		delete mWorld;
	}
	mWorld = NULL;
}

World* OpencraftCore::getWorld()
{
	return mWorld;
}

Character* OpencraftCore::createCharacter()
{
	Character* c = new Character();
	mCharacters.push_back( c );
	return c;
}

void OpencraftCore::destroyCharacter( Character* c )
{
	for( std::vector<Character*>::iterator it = mCharacters.begin(); it != mCharacters.end(); it++ ) {
		if( c == (*it) )
			mCharacters.erase( it++ );
	}
}

