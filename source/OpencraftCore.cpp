#include "OpencraftCore.h"
#include "WorldChunk.h"
#include "Renderer.h"
#include "TextureManager.h"

OpencraftCore* OpencraftCore::Singleton = 0;

OpencraftCore::OpencraftCore(void)
: mContinue( true ),
radius( 0 )
{
	OpencraftCore::Singleton = this;
	mRenderer = new Renderer();
	mTextureManager = new TextureManager();
}

OpencraftCore::~OpencraftCore(void)
{
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

	//Create some testing chunks
	createTestChunks( radius += 2 );

	int lastX = 0;
	int lastY = 0;

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
				(lEvt.Key.Code == sf::Key::Subtract) ) {
					createTestChunks( --radius );
			}
			if( (lEvt.Type == sf::Event::KeyPressed) &&
				(lEvt.Key.Code == sf::Key::Add) ) {
					createTestChunks( ++radius );
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
					mRenderer->getCamera().pitch( lEvt.MouseMove.Y  - lastY );
					mRenderer->getCamera().yaw( lEvt.MouseMove.X - lastX );
					lastY = lEvt.MouseMove.Y;
					lastX = lEvt.MouseMove.X;
			}
			if( lEvt.Type == sf::Event::Resized ) {
				mRenderer->resizeViewport( 0, 0, lEvt.Size.Width, lEvt.Size.Height );
			}
		}

		//Ensure each loaded chunk is updated before being sent to the GPU
		for( ChunkList::iterator it = mChunks.begin(); it != mChunks.end(); ++it ) {
			(*it)->update(lDelta);
		}
		mRenderer->render(lDelta, mChunks);

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

void OpencraftCore::createTestChunks( int size )
{
	destoryWorld();
	for(int i = -size; i < size; i++) {
		for(int z = -size; z < size; z++) {
			Util::log("New Chunk");
			createChunk(i,0,z);
		}
	}
}

void OpencraftCore::destoryWorld()
{
	for(ChunkList::iterator it = mChunks.begin(); it != mChunks.end(); )
	{
		Util::log("Deleted Chunk");
		mRenderer->notifyChunkUnloaded((*it));
		delete (*it);
		it = mChunks.erase( it );
	}
}

WorldChunk* OpencraftCore::getChunk(const long x, const long y, const long z)
{
	for(ChunkList::iterator it = mChunks.begin(); it != mChunks.end(); ++it)
	{
		if( x == (*it)->getX() && y == (*it)->getY() && z == (*it)->getZ() ) {
			return (*it);
		}
	}
	return NULL;
}

void OpencraftCore::createChunk(long x, long y, long z)
{
	Util::log( "Creating Chunk: " + Util::toString(x) + "," + Util::toString(y) + "," + Util::toString(z) );
	WorldChunk* newChunk = new WorldChunk(x, y, z);
	Util::log("Adding test data..");
	newChunk->fillWithTestData();
	mChunks.push_back(newChunk);
	Util::log( "Chunk Created" );
}

void OpencraftCore::removeChunk(long x, long y, long z)
{
	for(ChunkList::iterator it = mChunks.begin(); it != mChunks.end(); ++it)
	{
		if( x == (*it)->getX() && y == (*it)->getY() && z == (*it)->getZ() ) {
			Util::log("Deleted Chunk");
			mRenderer->notifyChunkUnloaded((*it));
			delete (*it);
			mChunks.erase( it );
			return;
		}
	}
}
