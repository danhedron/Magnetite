#include "OpencraftCore.h"
#include "WorldChunk.h"
#include "Renderer.h"
#include "TextureManager.h"
#include "StoneBlock.h"
#include "World.h"
#include "Character.h"
#include "BlockFactory.h"

OpencraftCore* OpencraftCore::Singleton = 0;

OpencraftCore::OpencraftCore(void)
: mContinue( true ),
mWorld( NULL ),
mPlayer( NULL ),
mTimescale( 1.f ),
mLastX( 0.f ),
mLastY( 0.f )
{
	OpencraftCore::Singleton = this;
	mRenderer = new Renderer();
	mTextureManager = new TextureManager();
}

OpencraftCore::~OpencraftCore(void)
{
	unloadWorld();

	delete mRenderer;
	delete mTextureManager;
}

void OpencraftCore::createWindow(int *argc, char **argv)
{
	sf::WindowSettings wnds;
	wnds.DepthBits = 24;
	wnds.DepthBits = 8;
	//wnds.AntialiasingLevel = 2;
	mWindow.Create(sf::VideoMode(800,600,32), "Opencraft", sf::Style::Close | sf::Style::Resize, wnds);
	mWindow.EnableKeyRepeat( false );
	//mWindow.UseVerticalSync(true);
	mRenderer->initialize(argc, argv);
	mTextureManager->initalize();
	mRenderer->resizeViewport(0,0,800,600);
}

void OpencraftCore::go(int *argc, char **argv) 
{
	createWindow(argc, argv);

	// Print factory list for testing
	FactoryManager::logFactories();

	int lastX = mWindow.GetWidth()/2;
	int lastY = mWindow.GetHeight()/2;

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
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::R) ) {
					if(mRenderer->getRenderMode() == Renderer::RENDER_SOLID)
						mRenderer->setRenderMode(Renderer::RENDER_WIRE);
					else
						mRenderer->setRenderMode(Renderer::RENDER_SOLID);
			}
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::A) ) {
					mPlayer->addMoveDelta( Vector3( -1.f, 0.f, 0.f ) );
			}
			if( (lEvt.Type == sf::Event::KeyReleased) &&
				(lEvt.Key.Code == sf::Key::A) ) {
					mPlayer->addMoveDelta( Vector3( 1.f, 0.f, 0.f ) );
			}
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::D) ) {
					mPlayer->addMoveDelta( Vector3( 1.f, 0.f, 0.f ) );
			}
			if( (lEvt.Type == sf::Event::KeyReleased) &&
				(lEvt.Key.Code == sf::Key::D) ) {
					mPlayer->addMoveDelta( Vector3( -1.f, 0.f, 0.f ) );
			}
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::W) ) {
					mPlayer->addMoveDelta( Vector3( 0.f, 0.f, -1.f ) );
			}
			if( (lEvt.Type == sf::Event::KeyReleased) &&
				(lEvt.Key.Code == sf::Key::W) ) {
					mPlayer->addMoveDelta( Vector3( 0.f, 0.f, 1.f ) );
			}
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::S) ) {
					mPlayer->addMoveDelta( Vector3( 0.f, 0.f, 1.f ) );
			}
			if( (lEvt.Type == sf::Event::KeyReleased) &&
				(lEvt.Key.Code == sf::Key::S) ) {
					mPlayer->addMoveDelta( Vector3( 0.f, 0.f, -1.f ) );
			}
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::LShift) ) {
					mPlayer->enableSprint( true );
			}
			if( (lEvt.Type == sf::Event::KeyReleased) &&
				(lEvt.Key.Code == sf::Key::LShift) ) {
					mPlayer->enableSprint( false );
			}
			if( (lEvt.Type == sf::Event::KeyReleased) &&
				(lEvt.Key.Code == sf::Key::F3) ) {
					mRenderer->toggleCameraFrustum();
			}
			if( (lEvt.Type == sf::Event::KeyReleased) &&
				(lEvt.Key.Code == sf::Key::F2) ) {
					mRenderer->setWorldVisible( !mRenderer->isWorldVisible() );
			}
			if( (lEvt.Type == sf::Event::KeyReleased) &&
				(lEvt.Key.Code == sf::Key::F) ) {
					mPlayer->enableFlying( !mPlayer->isFlying() );
					Util::log( (Util::toString(mPlayer->isFlying())) + " Flying");
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
					mPlayer->getCamera()->pitch( -(lEvt.MouseMove.Y  - lastY) );
					mPlayer->getCamera()->yaw( -(lEvt.MouseMove.X - lastX) );
					lastX = mWindow.GetWidth()/2;
					lastY = mWindow.GetHeight()/2;
					
					mWindow.SetCursorPosition( mWindow.GetWidth() / 2, mWindow.GetHeight() / 2 );
			}
			if( lEvt.Type == sf::Event::MouseButtonPressed && lEvt.MouseButton.Button == sf::Mouse::Left ) {
				placeEyeBlock();
			}
			if( lEvt.Type == sf::Event::MouseButtonPressed && lEvt.MouseButton.Button == sf::Mouse::Right ) {
				removeEyeBlock();
			}
			if( lEvt.Type == sf::Event::Resized ) {
				mRenderer->resizeViewport( 0, 0, lEvt.Size.Width, lEvt.Size.Height );
			}
		}

		lDelta *= mTimescale;

		//Ensure each loaded chunk is updated before being sent to the GPU
		mWorld->update( lDelta );

		// Update the player once the world update has finished.
		mPlayer->update( lDelta );

		//Ensure the renderer has the correct camera
		mRenderer->setCamera(mPlayer->getCamera());
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

float OpencraftCore::getTimescale()
{
	return mTimescale;
}

void OpencraftCore::newWorld( std::string name )
{
	unloadWorld();

	mWorld = new World();
	mWorld->newWorld( name );
	createCharacter();
}

void OpencraftCore::unloadWorld()
{
	if( mWorld != NULL ) {
		// world handles saving.
		delete mWorld;
		// If the player still exists, delete it.
		if( mPlayer != NULL ) {
			delete mPlayer;
		}
	}
	mPlayer = NULL;
	mWorld = NULL;
}

World* OpencraftCore::getWorld()
{
	return mWorld;
}

Character* OpencraftCore::createCharacter()
{
	Util::log("Size Of Block: " + Util::toString(sizeof BaseBlock));
	if( mPlayer == NULL )
		mPlayer = new Character();
	mWindow.SetSize(mWindow.GetWidth(), mWindow.GetHeight());
	mPlayer->setPosition( Vector3( 0.f, 150.f, 0.f )  );
	if( mWorld )
		mWorld->setPagingCamera( mPlayer->getCamera() );
	return mPlayer;
}

void OpencraftCore::removeEyeBlock()
{
	raycast_r ray = mPlayer->getEyeCast();
	ray = mWorld->raycastWorld(ray);
	if(ray.hit)
	{
		Vector3 cIndex = mWorld->worldToChunks( ray.worldHit );
		//Vector3 bIndex = mWorld->worldToBlock( ray.worldHit );
		WorldChunk* chunk = mWorld->getChunk( cIndex.x, cIndex.y, cIndex.z );
		if(chunk && ray.block) {
			chunk->removeBlockAt( ray.block->getX(), ray.block->getY(), ray.block->getZ() );
		}
	}
}

void OpencraftCore::placeEyeBlock()
{
	raycast_r ray = mPlayer->getEyeCast();
	ray = mWorld->raycastWorld(ray);
	if(ray.hit && ray.block)
	{
		Util::log( Util::toString( ray.worldHit + ray.hitNormal ) );
		Vector3 cIndex = mWorld->worldToChunks( ray.worldHit + ray.hitNormal );
		Vector3 bIndex = Vector3( ray.block->getX(), ray.block->getY(), ray.block->getZ() ) + ray.hitNormal;
		Util::log("Ray Hit: " + Util::toString( cIndex ) + " Normal: " + Util::toString( ray.hitNormal ) );
		WorldChunk* chunk = mWorld->getChunk( cIndex.x, cIndex.y, cIndex.z );
		if(chunk) {
			BaseBlock* block = FactoryManager::createBlock(	mRenderer->blockType );
			if( block != NULL ) {
				block->setPosition( bIndex );
				chunk->addBlockToChunk( block );
			}
		}
	}
}