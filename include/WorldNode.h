#ifndef _WORLDNODE_H_
#define _WORLDNODE_H_
#include "prerequisites.h"

class WorldChunk;


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

struct WorldNode
{
	WorldNode() { for(int i = 0; i < 4; i++) children[i] = NULL; }
	bool isChunk;

	/**
	 * Index in the world I guess
	 */
	NodeIndex index;

	/**
	 * Child pointers, starting top-left, clockwise.
	 */
	WorldNode* children[4];
};


#endif // _WORLDNODE_H_