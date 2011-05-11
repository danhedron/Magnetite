#include "OpencraftCore.h"
#include "WorldChunk.h"
#include "Renderer.h"
#include "TextureManager.h"
#include "StoneBlock.h"

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
				(lEvt.Key.Code == sf::Key::T) ) {
					//Force regen of all chunk geom.
					for( ChunkList::iterator it = mChunks.begin(); it != mChunks.end(); ++it ) {
						(*it)->generate();
					}
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
				raycast_r ray;
				ray.orig = mRenderer->getCamera().getPosition();
				ray.dir = (mRenderer->getCamera().getMatrix() * Vector3(0,0,1.0f)).normalize();
				ray = raycastWorld(ray);
				if(ray.hit)
				{
					long cX, cY, cZ, bX, bY, bZ;
					cX = ray.worldHit.x/CHUNK_WIDTH;
					cY = ray.worldHit.y/CHUNK_HEIGHT;
					cZ = ray.worldHit.z/CHUNK_WIDTH;
					bX = abs((int)ray.worldHit.x)%CHUNK_WIDTH;
					bY = abs((int)ceil(ray.worldHit.y))%CHUNK_HEIGHT;
					bZ = abs((int)ray.worldHit.z)%CHUNK_WIDTH;
					WorldChunk* chunk = getChunk( cX, cY, cZ );
					Util::log("Ray hit block: " + Util::toString(Vector3(bX,bY,bZ)) + " in chunk " + Util::toString(Vector3(cX,cY,cZ)));
					if(chunk)
						chunk->addBlockToChunk( new StoneBlock( bX, bY, bZ ) );
				}
			}
			if( lEvt.Type == sf::Event::MouseButtonPressed && lEvt.MouseButton.Button == sf::Mouse::Right ) {
				raycast_r ray;
				ray.orig = mRenderer->getCamera().getPosition();
				ray.dir = (mRenderer->getCamera().getMatrix() * Vector3(0,0,1.0f)).normalize();
				ray = raycastWorld(ray);
				if(ray.hit)
				{
					long cX, cY, cZ, bX, bY, bZ;
					cX = ray.worldHit.x/CHUNK_WIDTH;
					cY = ray.worldHit.y/CHUNK_HEIGHT;
					cZ = ray.worldHit.z/CHUNK_WIDTH;
					bX = (int)ray.worldHit.x%CHUNK_WIDTH;
					bY = (int)ray.worldHit.y%CHUNK_HEIGHT;
					bZ = (int)ray.worldHit.z%CHUNK_WIDTH;
					WorldChunk* chunk = getChunk( cX, cY, cZ );
					Util::log("Ray hit block: " + Util::toString(Vector3(bX,bY,bZ)) + " in chunk " + Util::toString(Vector3(cX,cY,cZ)));
					if(chunk)
						chunk->removeBlockAt( bX, bY, bZ );
				}
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
			delete (*it);
			mChunks.erase( it );
			return;
		}
	}
}

raycast_r OpencraftCore::raycastWorld(raycast_r &ray)
{
	Vector3 min, max;
	ChunkList hitChunks;
	for(ChunkList::iterator it = mChunks.begin(); it != mChunks.end(); ++it)
	{
		min = Vector3( (*it)->getX() * CHUNK_WIDTH, (*it)->getY() * CHUNK_HEIGHT,  (*it)->getZ() * CHUNK_WIDTH );
		max = Vector3( (*it)->getX() * CHUNK_WIDTH + CHUNK_WIDTH, (*it)->getY() * CHUNK_HEIGHT + CHUNK_HEIGHT,  (*it)->getZ() * CHUNK_WIDTH + CHUNK_WIDTH );
		raycastCube(ray, min, max);
		if(ray.hit)
			hitChunks.push_back((*it));
	}
	BlockList* blocks = NULL;
	ray.hit = false;
	std::vector<raycast_r> raycasts;
	for(ChunkList::iterator it = hitChunks.begin(); it != hitChunks.end(); ++it)
	{
		blocks = (*it)->getVisibleBlocks();
		for(BlockList::iterator block = blocks->begin(); block != blocks->end(); ++block) {
			min = Vector3( (*it)->getX() * CHUNK_WIDTH + (*block).second->getX() - 0.5f,
							(*it)->getY() * CHUNK_HEIGHT + (*block).second->getY() - 0.5f,
							(*it)->getZ() * CHUNK_WIDTH + (*block).second->getZ() - 0.5f );
			max = Vector3( (*it)->getX() * CHUNK_WIDTH + (*block).second->getX() + 0.5f,
							(*it)->getY() * CHUNK_HEIGHT + (*block).second->getY() + 0.5f,
							(*it)->getZ() * CHUNK_WIDTH + (*block).second->getZ() + 0.5f );
			raycast_r r = ray;
			raycastCube(r, min, max);
			if( r.hit == true )
				raycasts.push_back( r );
		}	
	}
	
	// Final pass, find closest hit.
	float m = std::numeric_limits<float>::max();
	raycast_r closest;
	for(std::vector<raycast_r>::iterator it = raycasts.begin(); it != raycasts.end(); ++it)
	{
		if((*it).i0 < m) {
			closest = (*it);
		}
	}

	return closest;
}

raycast_r& OpencraftCore::raycastCube(raycast_r &ray, Vector3& min, Vector3& max)
{
	float t0 = 0;
	float t1 = ray.maxDistance;
	for(int i = 0; i < 3; ++i)
	{
		float invRayDir = 1.f / ray.dir[i];
		float near_t = (min[i] - ray.orig[i]) * invRayDir;
		float far_t = (max[i] - ray.orig[i]) * invRayDir;
		if(near_t > far_t)				
		{					
			float temp = near_t;
			near_t = far_t;
			far_t = temp;
		}
		t0 = near_t > t0 ? near_t : t0;
		t1 = far_t < t1 ? far_t : t1;
		if(t0 > t1) 
		{
			ray.hit = false;
			return ray;
		}
	}			
	ray.i0 = t0;
	ray.i1 = t1;
	ray.hit = true;
	ray.worldHit = ray.orig + ray.dir*ray.i0;
	return ray;
}