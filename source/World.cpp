#include "World.h"
#include "WorldChunk.h"
#include "Renderer.h"
#include "Sky.h"
#include "Camera.h"
#include "StoneBlock.h"
#include "ChunkGenerator.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

World::World()
: mSky( NULL ),
mPagingCamera( NULL )
{	
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
	destoryWorld();	
}

float World::getLightColor( LightIndex light )
{
	return ( 0.1f + ( 0.9f * ( (float)light/256 ) ) );
}

void World::newWorld( std::string name )
{
	mWorldName = name;
	createWorld();
}

void World::loadWorld( std::string name )
{
	mWorldName = name;
	destoryWorld();

	std::string svPath = getSavePath();

#ifdef WIN32
	size_t startTick = GetTickCount();
#endif
	
	for( int x = -5; x < 5; x++ )
	{
		for( int z = -5; z < 5; z++ )
		{
			std::string chnkPath = svPath + "chunks\\" + Util::toString( x ) + "_0_" + Util::toString( z );	
			std::ifstream stream(chnkPath.c_str() , std::ios_base::in | std::ios_base::binary );

			createChunk( x, 0, z );
			WorldChunk* c = getChunk( x, 0, z );
			c->readFromStream( stream );

		}
	}

#ifdef WIN32
	size_t ticks = GetTickCount() - startTick;
	Util::log("\tLoaded in: " + Util::toString(ticks) + "ms");
#endif

}

void World::createWorld()
{
	createSky(200);
	//Create some testing chunks
	createTestChunks( 5 );
}

std::string World::getName()
{
	return mWorldName;
}

std::string World::getSavePath()
{
	char currPath[FILENAME_MAX];
	getcwd(currPath, sizeof currPath);
	return std::string(currPath) + "\\worlds\\" + mWorldName + "\\";
}

void World::createTestChunks( int size )
{
	destoryWorld();
	for(int i = -size; i < size; i++) {
		for(int z = -size; z < size; z++) {
			WorldChunk* c = createChunk(i,0,z);
			mGenerator->fillChunk( c );
		}
	}
}

void World::destoryWorld()
{
	for( NodeList::iterator it = mQuadTrees.begin(); it != mQuadTrees.end(); ++it) {
		WorldNode* node = *it;
		_deleteTree(node);
	}
	mQuadTrees.clear();
	for(ChunkList::iterator it = mChunks.begin(); it != mChunks.end(); )
	{
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

WorldChunk* World::createChunk(long x, long y, long z)
{
	WorldChunk* newChunk = new WorldChunk(x, y, z);
	mChunks.push_back(newChunk);
	WorldNode* node = getChunkNode(Vector3(x,y,z), true);
	if( node == NULL ) 
		Util::log( "NULL node" );
	else
		node->children[0] = (WorldNode*)newChunk;
	
	return newChunk;
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

void World::createWorldFolder()
{
	char currPath[FILENAME_MAX];
	getcwd(currPath, sizeof currPath);
	std::string path = currPath;
	if( mkdir( (path + "\\worlds\\").c_str() ) != ENOENT ) {
		if( mkdir( (path + "\\worlds\\" + mWorldName + "\\").c_str() ) != ENOENT ) {
			mkdir( (path + "\\worlds\\" + mWorldName + "\\chunks").c_str() );
		}
	}
}

void World::saveAllChunks()
{
	std::string svPath = getSavePath();
	
	Util::log("Saving chunks");

	// Ensure that the chunks folder exists
	createWorldFolder();
	
#ifdef WIN32
	size_t startTick = GetTickCount();
#endif

	for( ChunkList::iterator it = mChunks.begin(); it != mChunks.end(); it++ )
	{
		std::string chnkPath = svPath + "chunks\\" + Util::toString( (*it)->getX() ) + "_" + Util::toString( (*it)->getY() ) + "_" + Util::toString( (*it)->getZ() );
		std::ofstream out( chnkPath.c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc );
		if( out && out.good() ) 
		{
			(*it)->appendToStream( out );
			out.close();
		}
		else 
		{
			Util::log("Error Writing Chunk");
		}
	}

#ifdef WIN32
	size_t ticks = GetTickCount() - startTick;
	Util::log("\tSaved in: " + Util::toString(ticks) + "ms");
#endif

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

Vector3 normals[3] = {
	Vector3(1.f, 0.f, 0.f), // X
	Vector3(0.f, 1.f, 0.f), // Y
	Vector3(0.f, 0.f, 1.f) // Z
};

raycast_r World::raycastCube(const raycast_r &inray, Vector3& min, Vector3& max)
{
	raycast_r ray(inray);

	float Tnear = INT_MIN; // These values are plenty large enough.
	float Tfar	= INT_MAX;

	for(int p = 0; p < 3; p++) {
		Vector3 norm = normals[p]; 
		if( ray.dir[p] == 0 ) {
			// Ray is parallel to this plane
			if( ray.orig[p] < min[p] || ray.orig[p] > max[p] )
			{
				// Ray is outside of the planes of this axis, it can't intersect.
				ray.hit = false; 
				return ray;
			}
		}
		else
		{
			float t1 = (min[p] - ray.orig[p]) / ray.dir[p];
			float t2 = (max[p] - ray.orig[p]) / ray.dir[p];
			if( t1 > t2 ) {
				std::swap( t1, t2 );
			}
			else { norm = -norm; }
			if( t1 > Tnear ) {
				Tnear = t1;
				ray.hitNormal = norm;
			}
			if( t2 < Tfar ) Tfar = t2;
			if( Tnear > Tfar || Tfar < 0 ) {
				ray.hit = false;
				return ray;
			}
		}
	}

	ray.hitNormal = ray.hitNormal.normalize();

	ray.hit = true;
	ray.i0 = Tnear;
	ray.i1 = Tfar;
	ray.worldHit = ray.orig + ( ray.dir * ray.i0 );
	return ray;
}

Vector3 World::worldToChunks(const Vector3 &vec)
{
	Vector3 v;
	v.x = floor( vec.x / (float)CHUNK_WIDTH );
	v.y = floor( vec.y / (float)CHUNK_HEIGHT );
	v.z = floor( vec.z / (float)CHUNK_WIDTH );
	return v;
}

Vector3 World::worldToBlock(const Vector3 &vec)
{
	Vector3 v;
	v.x = ((int)vec.x % CHUNK_WIDTH);// * ( vec.x < 0 ? -1.f : 1.f );
	v.y = ((int)vec.y % CHUNK_HEIGHT);// * ( vec.y < 0 ? -1.f : 1.f );
	v.z = ((int)vec.z % CHUNK_WIDTH);// * ( vec.z < 0 ? -1.f : 1.f );
	return v;
}