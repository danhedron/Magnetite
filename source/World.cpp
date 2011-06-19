#include "World.h"
#include "WorldChunk.h"
#include "Renderer.h"
#include "Sky.h"
#include "Camera.h"
#include "StoneBlock.h"
#include "ChunkGenerator.h"

World::World()
: mSky( NULL ),
mPagingCamera( NULL )
{	
	createWorld();
	mGenerator = new ChunkGenerator(1024);
}

void _deleteTree( WorldNode* node )
{
	for( int c = 0; c < 4; c++ ) {
		if( node->children[c]->isChunk )
			delete node->children[c];
		else
			_deleteTree(node->children[c]);
	}
	delete node;
}

World::~World()
{

	for( NodeList::iterator it = mQuadTrees.begin(); it != mQuadTrees.end(); ++it) {
		WorldNode* node = *it;
		_deleteTree(node);
	}
}

void World::createWorld()
{
	createSky(200);
	//Create some testing chunks
	createTestChunks( 5 );
}

void World::createTestChunks( int size )
{
	destoryWorld();
	for(int i = -size; i < size; i++) {
		for(int z = -size; z < size; z++) {
			Util::log("New Chunk");
			createChunk(i,0,z);
		}
	}
}

void World::destoryWorld()
{
	for(ChunkList::iterator it = mChunks.begin(); it != mChunks.end(); )
	{
		Util::log("Deleted Chunk");
		delete (*it);
		it = mChunks.erase( it );
	}
}

ChunkList& World::getChunks()
{
	return mChunks;
}

WorldChunk* World::getChunk(const long x, const long y, const long z)
{
	for(ChunkList::iterator it = mChunks.begin(); it != mChunks.end(); ++it)
	{
		if( x == (*it)->getX() && y == (*it)->getY() && z == (*it)->getZ() ) {
			return (*it);
		}
	}
	return NULL;
}

NodeList& World::getTopNodes()
{
	return mQuadTrees;
}

void World::_populateTree(WorldNode* parent, const NodeIndex& ppos, int depth )
{
	if(depth > 8 ) return; // End of recursion

	NodeIndex ind = ppos;
	WorldNode* node = new WorldNode();
	node->index = ind;
	node->isChunk = (depth==8 ? true : false);
	_populateTree(node, ind, depth*2);
	parent->children[0] = node;

	NodeIndex ind2 = { ppos.x + NODE_SIZE/depth, ppos.y, ppos.z };
	node = new WorldNode();
	node->index = ind2;
	node->isChunk = (depth==8 ? true : false);
	_populateTree(node, ind2, depth*2);
	parent->children[1] = node;

	NodeIndex ind3 = { ppos.x + NODE_SIZE/depth, ppos.y, ppos.z + NODE_SIZE/depth };
	node = new WorldNode();
	node->index = ind3;
	node->isChunk = (depth==8 ? true : false);
	_populateTree(node, ind3, depth*2);
	parent->children[2] = node;
	
	NodeIndex ind4 = { ppos.x, ppos.y, ppos.z + NODE_SIZE/depth };
	node = new WorldNode();
	node->index = ind4;
	node->isChunk = (depth==8 ? true : false);
	_populateTree(node, ind4, depth*2);
	parent->children[3] = node;
}

WorldNode* World::getWorldNode( const Vector3& pos, bool safe )
{
	Vector3 nodepos = Vector3( (int)(floor(pos.x/NODE_SIZE))*NODE_SIZE,  (int)(pos.y),  (int)(floor(pos.z/NODE_SIZE))*NODE_SIZE );
	NodeIndex index = { nodepos.x, nodepos.y, nodepos.z };
	for( NodeList::iterator it = mQuadTrees.begin(); it != mQuadTrees.end(); ++it) {
		if( (*it)->index == index ) return *it;
	}
	if(!safe) { 
		return NULL;
	}


	WorldNode* node = new WorldNode();
	node->isChunk = false;
	node->index = index;
	mQuadTrees.push_back(node);

	// Populate tree
	_populateTree(node, index, 2);

	return node;
}

WorldNode* World::getChunkNode( const Vector3& pos, bool safe )
{
	// Find root level node.
	WorldNode* node = getWorldNode( pos, safe );
	Vector3 rootpos = Vector3( (int)(floor(pos.x/NODE_SIZE))*NODE_SIZE,  (int)(pos.y),  (int)(floor(pos.z/NODE_SIZE))*NODE_SIZE );

	for( int i = 2; i <= 8; i*=2 ) {
		int depthSize = NODE_SIZE/i;
		Vector3 nodepos = Vector3( (int)(floor(pos.x/depthSize))*depthSize,  (int)(pos.y), (int)(floor(pos.z/depthSize))*depthSize );
		NodeIndex index = { nodepos.x, nodepos.y, nodepos.z };
		WorldNode* child;
		for( int c = 0; c < 4; c++ ) {
			child = node->children[c];
			if( child->index == index ) {
				if( child->isChunk ) return child;
				else { node = child; break; }
			}
		}
	}
	
	return NULL;
}

std::string World::printTree()
{
	std::string buff = "";
	for( NodeList::iterator it = mQuadTrees.begin(); it != mQuadTrees.end(); ++it) {
		buff += _printTree(*it, 0);
	}
	return buff;
}

std::string World::_printTree(WorldNode* node, int depth)
{
	std::string buff = "";
	for(int i = 0; i < depth; i++ ) { buff += "\t"; }
	if( node->isChunk )
		buff += "[Chunk]";
	else
		buff += "[Node]";
	buff += " " + Util::toString(node->index.x) + " " + Util::toString(node->index.y) + " " + Util::toString(node->index.z);
	if( !node->isChunk )
		buff += " =>";
	else {
		std::stringstream ss;
		ss << (size_t)node->children[0];
		buff += " 0x" + ss.str();

	}
	buff += "\n";
	if(!node->isChunk) {
		for(int c = 0; c < 4; c++) {
			for(int i = 0; i < depth; i++ ) { buff += "\t"; }
			if( node->children[0] != NULL )
			buff += _printTree(node->children[c], depth+1);
		}
	}
	return buff;
}

void World::createChunk(long x, long y, long z)
{
	Util::log( "Creating Chunk: " + Util::toString(x) + "," + Util::toString(y) + "," + Util::toString(z) );
	WorldChunk* newChunk = new WorldChunk(x, y, z);
	mGenerator->fillChunk( newChunk );
	mChunks.push_back(newChunk);
	WorldNode* node = getChunkNode(Vector3(x,y,z), true);
	if( node == NULL ) 
		Util::log( "NULL node" );
	else
		node->children[0] = (WorldNode*)newChunk;
	
	Util::log( "Chunk Created" );
}

void World::setPagingCamera( Camera* _c )
{
	mPagingCamera = _c;
}

void World::activateChunk( long x, long y, long z )
{
	// Generate or load the chunk as it is not loaded.
	createChunk( x, y, z );
}

void World::deativateChunk( long x, long y, long z )
{
	removeChunk( x, y, z );
}

void World::update( float dt )
{
	if( mSky != NULL )
		mSky->update( dt );

	for( ChunkList::iterator it = mChunks.begin(); it != mChunks.end(); ++it ) {
		(*it)->update(dt);
	}

	for( ChunkList::iterator it = mChunks.begin(); it != mChunks.end(); ++it ) {
		(*it)->requestGenerate();
	}
}

void World::createSky( size_t time )
{
	if(mSky != NULL)
		delete mSky;
	mSky = new Sky();
	mSky->setTime( time );
}

Sky* World::getSky()
{
	return mSky;
}

void World::removeChunk(long x, long y, long z)
{
	for(ChunkList::iterator it = mChunks.begin(); it != mChunks.end(); ++it)
	{
		if( x == (*it)->getX() && y == (*it)->getY() && z == (*it)->getZ() ) {
			Util::log("Deleted Chunk");
			delete (*it);
			mChunks.erase( it );

			WorldNode* node = getChunkNode(Vector3(x,y,z), true);
			if( node == NULL ) 
				Util::log( "NULL node" );
			else
				node->children[0] = NULL;
			return;
		}
	}
}

raycast_r World::raycastWorld(const raycast_r &inray, bool solidOnly)
{
	raycast_r ray = inray;
	Vector3 min, max;
	ChunkList hitChunks;
	for(ChunkList::iterator it = mChunks.begin(); it != mChunks.end(); ++it)
	{
		min = Vector3( (*it)->getX() * CHUNK_WIDTH, (*it)->getY() * CHUNK_HEIGHT,  (*it)->getZ() * CHUNK_WIDTH );
		max = Vector3( (*it)->getX() * CHUNK_WIDTH + CHUNK_WIDTH, (*it)->getY() * CHUNK_HEIGHT + CHUNK_HEIGHT,  (*it)->getZ() * CHUNK_WIDTH + CHUNK_WIDTH );
		ray = raycastCube(ray, min, max);
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
			if( solidOnly && !(*block).second->isSolid() )
				continue;
			min = Vector3( (*it)->getX() * CHUNK_WIDTH + (*block).second->getX() - 0.0f,
							(*it)->getY() * CHUNK_HEIGHT + (*block).second->getY() - 0.0f,
							(*it)->getZ() * CHUNK_WIDTH + (*block).second->getZ() - 0.0f );
			max = Vector3( (*it)->getX() * CHUNK_WIDTH + (*block).second->getX() + 1.0f,
							(*it)->getY() * CHUNK_HEIGHT + (*block).second->getY() + 1.0f,
							(*it)->getZ() * CHUNK_WIDTH + (*block).second->getZ() + 1.0f );
			raycast_r r = inray;
			r = raycastCube(r, min, max);
			if( r.hit == true ) {
				r.block = block->second;
				raycasts.push_back( r );
			}
		}	
	}
	// Final pass, find closest hit.
	float m = std::numeric_limits<float>::max();
	raycast_r closest;
	for(std::vector<raycast_r>::iterator it = raycasts.begin(); it != raycasts.end(); ++it)
	{
		if((*it).i0 < m) {
			m = it->i0;
			closest = (*it);
		}
	}

	return closest;
}

collision_r World::AABBWorld(const collision_r& info)
{
	ChunkList hitChunks;
	for(ChunkList::iterator it = mChunks.begin(); it != mChunks.end(); ++it)
	{
		collision_r col = info;
		col.min2 = Vector3( (*it)->getX() * CHUNK_WIDTH, (*it)->getY() * CHUNK_HEIGHT,  (*it)->getZ() * CHUNK_WIDTH );
		col.max2 = Vector3( (*it)->getX() * CHUNK_WIDTH + CHUNK_WIDTH, (*it)->getY() * CHUNK_HEIGHT + CHUNK_HEIGHT,  (*it)->getZ() * CHUNK_WIDTH + CHUNK_WIDTH );
		col = AABBCube( col );
		if( col.collision )
			hitChunks.push_back( *it );
	}
	BlockList* blocks = NULL;
	collision_r coll;
	for(ChunkList::iterator it = hitChunks.begin(); it != hitChunks.end(); ++it)
	{
		blocks = (*it)->getVisibleBlocks();
		for(BlockList::iterator block = blocks->begin(); block != blocks->end(); ++block) {
			collision_r bcol = info;
			bcol.min2 = Vector3( (*it)->getX() * CHUNK_WIDTH + (*block).second->getX() - 0.0f,
							(*it)->getY() * CHUNK_HEIGHT + (*block).second->getY() - 0.0f,
							(*it)->getZ() * CHUNK_WIDTH + (*block).second->getZ() - 0.0f );
			bcol.max2 = Vector3( (*it)->getX() * CHUNK_WIDTH + (*block).second->getX() + 1.0f,
							(*it)->getY() * CHUNK_HEIGHT + (*block).second->getY() + 1.0f,
							(*it)->getZ() * CHUNK_WIDTH + (*block).second->getZ() + 1.0f );
			bcol = AABBCube( bcol );
			if( bcol.collision )
				coll.collision = true;
		}	
	}

	return coll;
}

collision_r World::AABBCube(const collision_r& info)
{
	collision_r collision = info;
	
	if( collision.max1.x < collision.min2.x )
		collision.collision = false;
	if( collision.max1.y < collision.min2.y )
		collision.collision = false;
	if( collision.max1.z < collision.min2.z )
		collision.collision = false;

	if( collision.min1.x > collision.max2.x )
		collision.collision = false;
	if( collision.min1.y > collision.max2.y )
		collision.collision = false;
	if( collision.min1.z > collision.max2.z )
		collision.collision = false;

	return collision;
}

raycast_r World::raycastCube(const raycast_r &inray, Vector3& min, Vector3& max)
{
	raycast_r ray(inray);
	const Vector3 l1((min - ray.orig) / ray.dir);
	const Vector3 l2((max - ray.orig) / ray.dir);

	Vector3 v_far;
	v_far.x = std::max( l1.x, l2.x );
	v_far.y = std::max( l1.y, l2.y );
	v_far.z = std::max( l1.z, l2.z );
	Vector3 v_near;
	v_near.x = std::min( l1.x, l2.x );
	v_near.y = std::min( l1.y, l2.y );
	v_near.z = std::min( l1.z, l2.z );
	ray.i1 = std::min( v_far.smallestDimension(), ray.maxDistance );
	ray.i0 = std::max( v_near.biggestDimension(), 0.f );
	ray.hit = (ray.i1 >= ray.i0) & (ray.i1 >= 0.f );
	ray.worldHit = ray.orig + ( ray.dir * ray.i0 );
	return ray;
}

Vector3 World::worldToChunks(const Vector3 &vec)
{
	Vector3 v;
	v.x = (vec.x / CHUNK_WIDTH) - ( vec.x < 0 ? 1 : 0 );
	v.y = (vec.y / CHUNK_HEIGHT) - ( vec.y < 0 ? 1 : 0 );
	v.z = (vec.z / CHUNK_WIDTH) - ( vec.z < 0 ? 1 : 0 );
	return v;
}

Vector3 World::worldToBlock(const Vector3 &vec)
{
	Vector3 v;
	v.x = ( vec.x < 0 ? CHUNK_WIDTH-1 : 0 ) + ( vec.x < 0 ? -1 : 1 ) * abs((int)vec.x)%CHUNK_WIDTH;
	v.y = ( vec.y < 0 ? CHUNK_HEIGHT-1 : 0 ) + ( vec.y < 0 ? -1 : 1 ) * abs((int)vec.y)%CHUNK_HEIGHT;
	v.z = ( vec.z < 0 ? CHUNK_WIDTH-1 : 0 ) + ( vec.z < 0 ? -1 : 1 ) * abs((int)vec.z)%CHUNK_WIDTH;
	return v;
}