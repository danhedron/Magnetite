#include "WorldTree.h"
#include "WorldNode.h"

WorldTree::WorldTree( const Vector3& pos )
	: mWorldPosition( pos ),
	tree( NULL )

{
	tree = createNode();
	tree->worldPosition = pos;
}

Vector3 WorldTree::getWorldPosition()
{
	return mWorldPosition;
}

WorldNode* WorldTree::createNode( )
{
	return new WorldNode(NULL);
}

bool WorldTree::isInside( const Vector3& pos )
{
	if( pos.x < mWorldPosition.x || pos.y < mWorldPosition.y || pos.z < mWorldPosition.z )
	{
		return false;
	}
	if( pos.x > mWorldPosition.x + CHUNK_SIZE || pos.y > mWorldPosition.y + CHUNK_SIZE || pos.z > mWorldPosition.z + CHUNK_SIZE )
	{
		return false;
	}
	return true;
}

WorldNode* WorldTree::getNodeAt( const Vector3& pos, bool create )
{
	Vector3 local = pos - mWorldPosition;
	// Find the node that contains this point	
	return tree->getNodeAt( local, create );
}
