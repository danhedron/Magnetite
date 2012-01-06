#ifndef _WORLDTREE_H_
#define _WORLDTREE_H_
#include "prerequisites.h"

class WorldTree 
{
protected:
	Vector3 mWorldPosition; // The position of the octree relative to the world origin.

	WorldNode* createNode(); // Creates a node of the given type.
public:
	WorldTree( const Vector3& pos );

	Vector3 getWorldPosition();

	WorldNode* tree; // The actual begining of the octree.

	bool isInside( const Vector3& pos ); // Returns true of the point is inside the tree

	WorldNode* getNodeAt( const Vector3& pos , bool create = false ); // Returns the node at this point, creating if specified
};


#endif
