#include "Tests.h"
#include "World.h"
#include "BaseBlock.h"
#include "WorldChunk.h"
#include "Renderer.h"
#include "WorldTree.h"
#include "WorldNode.h"

int tests = 0, failed = 0;

void _fireAssert( bool condition, std::string name, std::string loc )
{
	tests++;
	if(!condition) 
	{
		failed++;
		Util::log(name + " - " + loc);
	}
}
#define _str(x) #x
#define _ass( cond, error ) \
	_fireAssert( cond, error, "("__FILE__" at line " + Util::toString(__LINE__) + ")");

void runTests()
{
	// Create a world node
	WorldNode* wn = new WorldNode( NULL );

	WorldNode* cn = wn->getNodeAt( Vector3( 0, 0, 0 ), true );

	//_ass( cn != NULL, "Node at (0,0,0) is NULL!");
	//_ass( !cn->isChunk(), "Node at (0,0,0) is not a NULL chunk" );

	// A test chunk
	cn->chunk = cn->_createChunk();

	//_ass( cn->isChunk(), "Chunk creation failed!" );

	// get some other node
	WorldNode* nd2 = wn->getNodeAt( Vector3( 16, 0, 32 ), true );
	//_ass( nd2 != NULL, "Node Is null");
	nd2->chunk = nd2->_createChunk();

	// Create a world tree
	WorldTree* tree = new WorldTree(Vector3(0,0,0));
	WorldNode* node = tree->getNodeAt( Vector3( 32, 0, 32 ) );
	node->chunk = node->_createChunk();

	//_ass( Vector3(32, 0, 32) == node->worldPosition, "Node positition incorrect");

}
