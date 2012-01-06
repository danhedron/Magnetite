#ifndef _WORLDNODE_H_
#define _WORLDNODE_H_
#include "prerequisites.h"

#include "Chunk.h"

/**
 * Structure for storing a Node's Index, it's position in the parent's array.
 */
struct NodeIndex {
	long x;
	long y;
	long z;
	bool operator ==(NodeIndex& rhs) { return (x==rhs.x && y==rhs.y && z==rhs.z); }
};

/**
 * @struct WorldNode
 * Provides a simple structure for storing Quadtree data
 */

class WorldNode
{
	WorldNode* parent;

	Chunk* createChunk( ChunkIndex i );
	WorldNode* createNode( NodeIndex idx ) { return new WorldNode( this ); }
public:

	WorldNode( WorldNode* parent );

	/**
	 * Index in relation to the parent node
	 */
	NodeIndex index;

	/**
	 * World Position of the node
	 */
	Vector3 worldPosition;

	/**
	 * Depth in the scene graph
	 */
	size_t depth();

	/**
	 * returns true if the position (relative to the parent) is inside 
	 */
	bool isInside( const Vector3& pos );

	/**
	 * returns the node containing the given point, creating it if neccasary
	 */
	WorldNode* getNodeAt( const Vector3& pos, bool create = false );

	/**
	 * Returns the node at the given Chunk Index
	 */
	WorldNode* getNodeAt( const ChunkIndex& idx, bool create = false );

	/**
	 * Returns the chunk at the given Index
	 */
	Chunk* getChunkAt( const ChunkIndex& idx, bool create = false );

	/**
	 * Child pointers, starting top-left, clockwise.
	 */
	WorldNode* children[8];

	/**
	 * Node chunks
	 */
	Chunk* chunk;

	Chunk* _createChunk();

	/**
	 * returns true if this node contains chunks, false otherwise
	 */
	bool isChunk();
};


#endif // _WORLDNODE_H_
