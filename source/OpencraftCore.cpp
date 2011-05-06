#include "OpencraftCore.h"
#include "WorldChunk.h"
#include "Renderer.h"


OpencraftCore::OpencraftCore(void)
: mContinue( true )
{
	mRenderer = new Renderer();
}

OpencraftCore::~OpencraftCore(void)
{
}

void OpencraftCore::createWindow()
{
	sf::WindowSettings wnds;
	wnds.DepthBits = 24;
	wnds.DepthBits = 8;
	//wnds.AntialiasingLevel = 2;
	mWindow.Create(sf::VideoMode(800,600,32), "Opencraft", sf::Style::Close, wnds);
	mWindow.UseVerticalSync(true);
	mRenderer->initialize();
}

void OpencraftCore::go() 
{
	createWindow();

	//Create some testing chunks
	createChunk(0,0,0);

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
			if( lEvt.Type == sf::Event::Resized ) {
				mRenderer->resizeViewport( 0, 0, lEvt.Size.Width, lEvt.Size.Height );
			}
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

void OpencraftCore::createChunk(long x, long y, long z)
{
	WorldChunk* newChunk = new WorldChunk(x, y, z);
	newChunk->fillWithTestData();
	mChunks.push_back(newChunk);
}