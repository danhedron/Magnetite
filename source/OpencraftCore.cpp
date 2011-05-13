#include "OpencraftCore.h"
#include "WorldChunk.h"
#include "Renderer.h"
#include "TextureManager.h"
#include "StoneBlock.h"
#include "World.h"

OpencraftCore* OpencraftCore::Singleton = 0;

OpencraftCore::OpencraftCore(void)
: mContinue( true ),
mWorld( NULL )
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
	//mWindow.UseVerticalSync(true);
	mRenderer->initialize(argc, argv);
	mTextureManager->initalize();
	mRenderer->resizeViewport(0,0,800,600);
}

void OpencraftCore::go(int *argc, char **argv) 
{
	createWindow(argc, argv);

	int lastX = mWindow.GetWidth()/2;
	int lastY =  mWindow.GetHeight()/20;

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
					mRenderer->getCamera().translate(Vector3(-0.2f,0,0));
			}
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::D) ) {
					mRenderer->getCamera().translate(Vector3(0.2f,0,0));
			}
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::W) ) {
					mRenderer->getCamera().translate(Vector3(0,0,-0.2f));
			}
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::S) ) {
					mRenderer->getCamera().translate(Vector3(0,0,0.2f));
			}
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::PageUp) ) {
					mRenderer->getCamera().translate(Vector3(0,0.2f,0));
			}
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::PageDown) ) {
					mRenderer->getCamera().translate(Vector3(0,-0.2f,0));
			}
			if( (lEvt.Type == sf::Event::MouseMoved) ) {
					mRenderer->getCamera().pitch( -(lEvt.MouseMove.Y  - lastY) );
					mRenderer->getCamera().yaw( -(lEvt.MouseMove.X - lastX) );
					lastY = lEvt.MouseMove.Y;
					lastX = lEvt.MouseMove.X;
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

		//Ensure each loaded chunk is updated before being sent to the GPU
		for( ChunkList::iterator it = mWorld->getChunks().begin(); it != mWorld->getChunks().end(); ++it ) {
			(*it)->update(lDelta);
		}
		mRenderer->render(lDelta, mWorld->getChunks());

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

void OpencraftCore::newWorld( std::string name )
{
	unloadWorld();

	mWorld = new World();
	// Creates a testing world for now.
	mWorld->createWorld();
}

void OpencraftCore::unloadWorld()
{
	if( mWorld != NULL ) {
		// world handles saving.
		delete mWorld;
	}
	mWorld = NULL;
}

void OpencraftCore::removeEyeBlock()
{
	raycast_r ray;
	ray.orig = mRenderer->getCamera().getPosition();
	ray.dir = (mRenderer->getCamera().getMatrix() * Vector3(0,0,1.0f)).normalize();
	ray = mWorld->raycastWorld(ray);
	if(ray.hit)
	{
		long cX, cY, cZ, bX, bY, bZ;
		cX = ray.worldHit.x/CHUNK_WIDTH;
		cY = ray.worldHit.y/CHUNK_HEIGHT;
		cZ = ray.worldHit.z/CHUNK_WIDTH;
		bX = (int)ray.worldHit.x%CHUNK_WIDTH;
		bY = (int)ray.worldHit.y%CHUNK_HEIGHT;
		bZ = (int)ray.worldHit.z%CHUNK_WIDTH;
		WorldChunk* chunk = mWorld->getChunk( cX, cY, cZ );
		Util::log("Ray hit block: " + Util::toString(Vector3(bX,bY,bZ)) + " in chunk " + Util::toString(Vector3(cX,cY,cZ)));
		if(chunk)
			chunk->removeBlockAt( bX, bY, bZ );
	}

}

void OpencraftCore::placeEyeBlock()
{
	raycast_r ray;
	ray.orig = mRenderer->getCamera().getPosition();
	ray.dir = (mRenderer->getCamera().getMatrix() * Vector3(0,0,1.0f)).normalize();
	ray = mWorld->raycastWorld(ray);
	if(ray.hit)
	{
		long cX, cY, cZ, bX, bY, bZ;
		cX = ray.worldHit.x/CHUNK_WIDTH;
		cY = ray.worldHit.y/CHUNK_HEIGHT;
		cZ = ray.worldHit.z/CHUNK_WIDTH;
		bX = abs((int)ray.worldHit.x)%CHUNK_WIDTH;
		bY = abs((int)ceil(ray.worldHit.y))%CHUNK_HEIGHT;
		bZ = abs((int)ray.worldHit.z)%CHUNK_WIDTH;
		WorldChunk* chunk = mWorld->getChunk( cX, cY, cZ );
		Util::log("Ray hit block: " + Util::toString(Vector3(bX,bY,bZ)) + " in chunk " + Util::toString(Vector3(cX,cY,cZ)));
		if(chunk)
			chunk->addBlockToChunk( new StoneBlock( bX, bY, bZ ) );
	}
}