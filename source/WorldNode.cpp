#include "WorldNode.h"

/*
 * INLINE MATHS TIME!
 * Chunk Index: ( 80, 0, 80 )
 * First Level Node Size: 512 / 2 = 256
 * First Level Node: ( 0, 0, 0 )
 * Floored Chunk Index: floor( 80/256 ) 
 * Second Level Node Size: 256 / 2 = 128
 * Second Level Node: ( 0, 0, 0 )
 * Third Level Node Size: 128 / 2 = 64
 * Third Level Node: ( 64, 0, 64 )
 * Fourth Level Node Size: 64 / 2 = 32
 * Fourth Level Node: ( 64, 0, 64 )
 * Fifth Level Node Size: 32 / 2 = 16
 * Fifth Level Node: ( 80, 0, 80 )
 */

/**
 * How are Nodes Organized?
 * Bottom Layer:
 * (0,0,0) 	(0,0,size/2) 
 * [0]		[1]
 * (size/2,0,0) (size/2,0,size/2)
 * [2]		[3]
 * Top Layer:
 * (0,size/2,0)	(0,size/2,size/2)
 * [4]		[5]
 * (size/2,0,0) (size/2,size/2,size/2)
 *
 */

WorldNode::WorldNode( WorldNode* parent )
{
	this->parent = parent;
	this->chunk = NULL;
	for(int i = 0; i < 8; i++) 
	{
		children[i] = NULL;
	}
}

Chunk* WorldNode::createChunk( ChunkIndex i )
{
	return new Chunk( i );
}

Chunk* WorldNode::_createChunk( )
{
	ChunkIndex ci = ChunkIndex { (int)worldPosition.x, (int)worldPosition.y, (int)worldPosition.z };
	Chunk* c = createChunk( ci );
	return c;
}

size_t WorldNode::depth()
{
	if( parent == NULL ) return 0;
	return parent->depth() + 1;
}

WorldNode* WorldNode::getNodeAt( const Vector3& pos , bool create )
{
	size_t size = TREE_LENGTH / pow(2, (depth()+1));
	/*if( !isInside( pos ) )
	{
		Util::log("Point: " + Util::toString(pos) + " not inside " + Util::toString( worldPosition ) + " and " + Util::toString( worldPosition + Vector3( size, size, size ) ) );
		return NULL;
	}*/
	if( isChunk() || depth() == 5 ) 
	{
		return this;
	}

	Vector3 local = pos - worldPosition;
	NodeIndex ndi = { (int)floor( local.x / size ), (int)floor( local.y / size), (int)floor( local.z / size ) }; 
	Util::log( Util::toString( Vector3( index.x, index.y, index.z ) ) + " L" + Util::toString( local ) + " W" + Util::toString(worldPosition) + " P" + Util::toString(pos) );
	Util::log( Util::toString( TREE_LENGTH ) + " " + Util::toString( (depth()+1) ) + " " + Util::toString( size) + " " +  Util::toString( ndi.x ) + Util::toString( ndi.y ) + Util::toString( ndi.z ) );
	size_t childIndex = 0;
	if( ndi.x == 0 && ndi.y == 0 && ndi.z == 0 )
		childIndex = 0;
	else if( ndi.x == 0 && ndi.y == 0 && ndi.z == 1 )
		childIndex = 1;
	else if( ndi.x == 0 && ndi.y == 1 && ndi.z == 0 )
		childIndex = 2;
	else if( ndi.x == 0 && ndi.y == 1 && ndi.z == 1 )
		childIndex = 3;
	else if( ndi.x == 1 && ndi.y == 0 && ndi.z == 0 )
		childIndex = 4;
	else if( ndi.x == 1 && ndi.y == 0 && ndi.z == 1 )
		childIndex = 5;
	else if( ndi.x == 1 && ndi.y == 1 && ndi.z == 0 )
		childIndex = 6;
	else if( ndi.x == 1 && ndi.y == 1 && ndi.z == 1 )
		childIndex = 7;

	if( children[childIndex] == NULL && create )
	{
		WorldNode* nd = new WorldNode(this);
		nd->index = ndi;
		nd->worldPosition = this->worldPosition + Vector3( ndi.x * size, ndi.y * size, ndi.z * size );
		children[childIndex] = nd;
	}
	if( children[childIndex] == NULL )
	{
		return NULL;
	}

	return children[childIndex]->getNodeAt( pos, create );
}

Chunk* WorldNode::getChunkAt( const ChunkIndex& idx, bool create )
{

}

bool WorldNode::isChunk()
{
	return chunk != NULL;
}

bool WorldNode::isInside( const Vector3& pos )
{
	size_t size = TREE_LENGTH/(pow(2,depth()+1));
	if( pos.x < index.x * size || pos.y < index.y * size || pos.z < index.z * size )
	{
		return false; // Nope
	}
	if( pos.x > index.x * size + size || pos.y > index.y * size + size || pos.z > index.z * size + size )
	{
		return false;
	}
	// It isn't outside the node, so it's probably inside the node.
	return true;
}

WorldNode* WorldNode::getNodeAt( const ChunkIndex& idx, bool create )
{
	size_t size = pow( 2, 9 - depth() );
	float x = floor( (float)idx.x / size );
	float y = floor( (float)idx.y / size );
	float z = floor( (float)idx.z / size );

	size_t arryIdx = 0;

	if( x == 0 && y == 0 && z == 0 )
	{
		arryIdx = 0;
	}
	else if( x == 0 && y == 0 && z == 1 )
	{
		arryIdx = 1;
	}
	else if( x == 1 && y == 0 && z == 0 )
	{
		arryIdx = 2;
	}
	else if( x == 1 && y == 0 && z == 1 )
	{
		arryIdx = 3;
	}
	else if( x == 0 && y == 1 && z == 0 )
	{
		arryIdx = 4;
	}
	else if( x == 0 && y == 1 && z == 1 )
	{
		arryIdx = 5;
	}
	else if( x == 1 && y == 1 && z == 0 )
	{
		arryIdx = 6;
	}
	else if( x == 1 && y == 1 && z == 1 )
	{
		arryIdx = 7;
	}

	if( children[arryIdx] == NULL && create )
	{
		children[arryIdx] = createNode(NodeIndex { 
			(int)(index.x + x * size),
			(int)(index.y + y * size),
			(int)(index.z + z * size) 
		});
		return children[arryIdx];
	}
	else if( children[arryIdx] != NULL )
	{
		return children[arryIdx];
	}
	
	
}
