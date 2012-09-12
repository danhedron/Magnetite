#include "ScriptWrapper.h"
#include "MagnetiteCore.h"
#include "WorldChunk.h"
#include "Renderer.h"
#include "TextureManager.h"
#include "InputManager.h"
#include "StoneBlock.h"
#include "World.h"
#include "Player.h"
#include "Character.h"
#include "BlockFactory.h"
#include "BaseGame.h"
#include "ScriptGame.h"
#include "BulletDebug.h"
#include "ResourceManager.h"
#include <BaseEntity.h>
#include <Components/InfoComponent.h>
#include <Components/PhysicsComponent.h>
#include <Components/RenderableComponent.h>
#include <ModelResource.h>
#include <ProgramResource.h>
#include <ctime>
#include <thread>

MagnetiteCore* MagnetiteCore::Singleton = 0;
static GLDebugDrawer debug;


/* Simple events */
void globalEventHandler( const InputEvent& evt )
{
	MagnetiteCore::Singleton->getGame()->_inputEvents( evt );
	if( evt.event == Inputs::SCREENSHOT && evt.down == true ) {
		MagnetiteCore::Singleton->screenshot();
	}
}

MagnetiteCore::MagnetiteCore(void)
:
mResourceManager( NULL ),
mScriptWrapper( NULL ),
mTextureManager( NULL ),
mInputManager( NULL ),
mWorld( NULL ),
mContinue( true ),
mTimescale( 1.f ),
mPBroadphase( NULL ),
mPCConfig( NULL ),
mCCDispatch ( NULL ),
mSolver( NULL ),
mPhysicsWorld( NULL ),
mLastX( 0.f ),
mLastY( 0.f )
{
	MagnetiteCore::Singleton = this;
	mRenderer = new Renderer();
	mTextureManager = new TextureManager();
	mInputManager = new InputManager();
	mScriptWrapper = new ScriptWrapper();
	mInputManager->setEventCallback( Inputs::FORWARD, &globalEventHandler );
	mInputManager->setEventCallback( Inputs::BACK, &globalEventHandler );
	mInputManager->setEventCallback( Inputs::LEFT, &globalEventHandler );
	mInputManager->setEventCallback( Inputs::RIGHT, &globalEventHandler );
	mInputManager->setEventCallback( Inputs::JUMP, &globalEventHandler );
	mInputManager->setEventCallback( Inputs::SPRINT, &globalEventHandler );
	mInputManager->setEventCallback( Inputs::SCREENSHOT, &globalEventHandler );
	mInputManager->setEventCallback( Inputs::FLY, &globalEventHandler );
	mInputManager->setEventCallback( Inputs::RESPAWN, &globalEventHandler );
}

MagnetiteCore::~MagnetiteCore(void)
{
	unloadWorld();

	delete mRenderer;
	delete mResourceManager;
	delete mTextureManager;
	delete mInputManager;
	delete mScriptWrapper;

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

void MagnetiteCore::init(int *argc, char **argv)
{
	mResourceManager = new ResourceManager();
	mResourceManager->addLocation("./resources/shaders/");
	mResourceManager->addLocation("./resources/sprites/");
	mResourceManager->addLocation("./resources/ui/");
	mResourceManager->addLocation("./resources/models/");
	sf::ContextSettings wnds;
	wnds.depthBits = 24;
	//wnds.AntialiasingLevel = 2;
	mWindow.create(sf::VideoMode(800,600,32), "Magnetite", sf::Style::Close | sf::Style::Resize, wnds);
	mWindow.setKeyRepeatEnabled( false );
	mWindow.setVerticalSyncEnabled( false );
	mRenderer->initialize(mWindow);
	mTextureManager->initalize();
	mRenderer->resizeViewport(0,0,800,600);
	initalizePhysics();
}

void MagnetiteCore::initalizePhysics()
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

btDiscreteDynamicsWorld* MagnetiteCore::getPhysicsWorld()
{
	return mPhysicsWorld;
}

void MagnetiteCore::screenshot()
{
	sf::Image screen = mWindow.capture();
	time_t rawt = time( NULL );
	tm* t = localtime( &rawt );
	char tstr[80];
	strftime(tstr, 80, "%Y-%m-%d_%H-%M-%S", t);
	std::string fname("screenshot_");
	fname.append( tstr );
	fname.append(".png");
	Util::log("Saving screenshot to: " + fname);
	screen.saveToFile(fname);
}

void MagnetiteCore::startGame( const std::string& type )
{
	mScriptWrapper->init();
	
	//mGame = FactoryManager::getManager().createGame(type);
	ScriptGame* g = new ScriptGame();
	g->setName("script");
	g->_setScriptObject( mScriptWrapper->newGame(type) );
	mGame = g;
	if( mGame != NULL ) 
		Util::log("Starting game: " + mGame->getName() );
	mGame->_startGameSingle();
	
	newWorld("test");
	
	// Create a new entity
	auto ent = mWorld->createEntity<Magnetite::BaseEntity>();
	auto phys = ent->addComponent<Magnetite::PhysicsComponent>();
	auto vis = ent->addComponent<Magnetite::RenderableComponent>();
	
	vis->setModel( mResourceManager->getResource<ModelResource>( "drone.obj" ) );
	vis->setProgram( mResourceManager->getResource<ProgramResource>( "model.prog" ));

	phys->setTransform(btTransform( btQuaternion(), btVector3(5.f, 115.f, 5.f) ));
	phys->setMass(0.f);
	phys->create();
	
	mGame->_loadGame();
	
	mScriptWrapper->runFile("./scripts/main.js");
	
	// No multiplayer yet so just force player join
	mGame->_playerJoined();
}

void MagnetiteCore::inputMovement( const Vector3 &v )
{
	mGame->_inputMovement( v );
}

void MagnetiteCore::mouseMoved( const float x, const float y )
{
	mGame->_mouseMoved( x, y );
}

void MagnetiteCore::go() 
{
	int lastX = mWindow.getSize().x/2;
	int lastY = mWindow.getSize().y/2;
	
	mRenderer->setCamera( mGame->getLocalPlayer()->getCamera() );
	
	std::thread rendering_thread( [&]() {
		sf::Clock timer;
		
		while(mContinue && mWindow.isOpen()) {
			float lDelta = ((float)timer.getElapsedTime().asMilliseconds())/1000;
			
			if( lDelta < 1.f/60.f )
			{
				continue;
			}
			timer.restart(); 
			
			lDelta *= mTimescale;
			
			mPhysicsWorld->stepSimulation( lDelta );
			
			//Ensure each loaded chunk is updated before being sent to the GPU
			mWorld->update( lDelta );
		}
	});
	
	rendering_thread.detach();
	
	mClock.restart();
	while(mContinue && mWindow.isOpen()) {
		float lDelta = ((float)mClock.getElapsedTime().asMilliseconds())/1000;
		mClock.restart();
		
		// Handle Events before we do anything
		sf::Event lEvt;
		while( mWindow.pollEvent(lEvt) ) {
			if( lEvt.type == sf::Event::Closed ) {
				exit();
			}
			if( (lEvt.type == sf::Event::KeyPressed) &&
				(lEvt.key.code == sf::Keyboard::Escape) ) {
				exit();
			}

			if( lEvt.type == sf::Event::KeyPressed )
			{
				mInputManager->keyDown( lEvt.key.code );
				mGame->keyDown( lEvt.key.code );
			}
			if( lEvt.type == sf::Event::KeyReleased )
			{
				mInputManager->keyUp( lEvt.key.code );
				mGame->keyUp( lEvt.key.code );
			}

			if( (lEvt.type == sf::Event::KeyPressed) &&
				(lEvt.key.code == sf::Keyboard::R) ) {
					if(mRenderer->getRenderMode() == Renderer::RENDER_SOLID)
						mRenderer->setRenderMode(Renderer::RENDER_LIGHTING);
					else
						mRenderer->setRenderMode(Renderer::RENDER_SOLID);
			}

			//==========
			//= Debug Buttons
			//==========
			if( (lEvt.type == sf::Event::KeyReleased ) &&
				(lEvt.key.code == sf::Keyboard::F1 ) ) {
				mRenderer->setDebugMode( Renderer::DEBUG_OFF );
			}
			if( (lEvt.type == sf::Event::KeyReleased ) &&
				(lEvt.key.code == sf::Keyboard::F3 ) ) {
				mRenderer->setDebugMode( Renderer::DEBUG_STATS );
			}
			if( (lEvt.type == sf::Event::KeyReleased ) &&
				(lEvt.key.code == sf::Keyboard::F4 ) ) {
				mRenderer->setDebugMode( Renderer::DEBUG_PHYSICS );
			}
			if( (lEvt.type == sf::Event::KeyReleased ) &&
				(lEvt.key.code == sf::Keyboard::F5 ) ) {
				mRenderer->setDebugMode( Renderer::DEBUG_LINES );
			}
			if( (lEvt.type == sf::Event::KeyReleased ) &&
				(lEvt.key.code == sf::Keyboard::F6 ) ) {
				mRenderer->setDebugMode( Renderer::DEBUG_SKY );
			}
			if( (lEvt.type == sf::Event::KeyPressed) &&
				(lEvt.key.code == sf::Keyboard::Num9) ) {
					mTimescale *= 0.5f;
			}
			if( (lEvt.type == sf::Event::KeyPressed) &&
				(lEvt.key.code == sf::Keyboard::Num0) ) {
					mTimescale *= 2.f;
			}
			if( (lEvt.type == sf::Event::MouseMoved) ) {
					mGame->_mouseMoved( -(lEvt.mouseMove.x - lastX), -(lEvt.mouseMove.y  - lastY));
					lastX = lEvt.mouseMove.x;
					lastY = lEvt.mouseMove.y;
			}
			if( (lEvt.type == sf::Event::MouseLeft) ) {
					sf::Mouse::setPosition( sf::Vector2i( mWindow.getSize().x / 2, mWindow.getSize().y / 2), mWindow );
			}
			if( lEvt.type == sf::Event::MouseButtonPressed && lEvt.mouseButton.button == sf::Mouse::Left ) {
				mGame->_primary();
			}
			if( lEvt.type == sf::Event::MouseButtonPressed && lEvt.mouseButton.button == sf::Mouse::Right ) {
				mGame->_secondary();
			}
			if( lEvt.type == sf::Event::Resized ) {
				mRenderer->resizeViewport( 0, 0, lEvt.size.width, lEvt.size.height );
			}
		}
		
		if( mGame != NULL )
		{
			mGame->think( lDelta );
		}
		
		// Update all the characters
		for( std::vector<Character*>::iterator it = mCharacters.begin(); it != mCharacters.end(); it++ )
		{
			(*it)->update( lDelta );
		}
		
		
		mRenderer->render(lDelta, mWorld);
		
		mGame->uiPaint( mRenderer );

		mWindow.display();
	}

	mWindow.close();

}

void MagnetiteCore::exit()
{
	mContinue = false;
}

TextureManager* MagnetiteCore::getTextureManager()
{
	return mTextureManager;
}

ResourceManager* MagnetiteCore::getResourceManager()
{
	return mResourceManager;
}

Renderer* MagnetiteCore::getRenderer()
{
	return mRenderer;
}

ScriptWrapper* MagnetiteCore::getScriptManager()
{
	return mScriptWrapper;
}

BaseGame* MagnetiteCore::getGame()
{
	return mGame;
}

float MagnetiteCore::getTimescale()
{
	return mTimescale;
}

void MagnetiteCore::newWorld( std::string name )
{
	unloadWorld();

	mWorld = new World( 10 );
	mWorld->buildTerrain();
}

void MagnetiteCore::unloadWorld()
{
	if( mWorld != NULL ) {
		// world handles saving.
		delete mWorld;
	}
	mWorld = NULL;
}

World* MagnetiteCore::getWorld()
{
	return mWorld;
}

Character* MagnetiteCore::createCharacter()
{
	Character* c = new Character();
	mCharacters.push_back( c );
	return c;
}

void MagnetiteCore::addPlayer( Player* p )
{
	mCharacters.push_back( p );
}

void MagnetiteCore::destroyCharacter( Character* c )
{
	for( std::vector<Character*>::iterator it = mCharacters.begin(); it != mCharacters.end(); it++ ) {
		if( c == (*it) ) {
			mCharacters.erase( it++ );
			delete c;
		}
	}
}

