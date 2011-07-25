#include "LightingManager.h"
#include "WorldChunk.h"

LightingManager::LightingManager()
: litChunks(0)
{
}

LightingManager::~LightingManager()
{
}

void LightingManager::lightChunk( WorldChunk* chunk )
{
	for( size_t x = 0; x < CHUNK_WIDTH; x++ ) 
	{
		for( size_t z = 0; z < CHUNK_WIDTH; z++ ) 
		{
			lightColumn( chunk, x,  z, WorldChunk::Sunlight );
		}
	}
}

void LightingManager::lightColumn( WorldChunk* chunk, size_t x, size_t z, LightIndex light )
{
	size_t currentLight = WorldChunk::Sunlight;
	char directionFlags = 0; // Stores directions in which take us under cover.
	bool oh_my_its_dark_in_here = false; // Have we gone under-cover
	for( short y = CHUNK_HEIGHT - 1; y >= 0; y-- )
	{
		if( chunk->getBlockAt(x, y, z) ) 
		{
			chunk->setLightLevel( x, y, z, 0 );
			oh_my_its_dark_in_here = true;
		}
		else if( !oh_my_its_dark_in_here )
		{
			// Sunlight
			chunk->setLightLevel( x, y, z, WorldChunk::Sunlight );
		}
		else
		{
			chunk->setLightLevel( x, y, z, 0 );
		}

		//Check -X 
		if( chunk->getBlockAt(x-1, y, z) )
		{
			directionFlags |= NEG_X;
		}
		else if( (directionFlags & NEG_X) == NEG_X )
		{
			// light shadow
			lightUnderground( chunk, x - 1, y, z, NEG_X, WorldChunk::Sunlight & 1 );
		}

		//Check +X 
		if( chunk->getBlockAt(x+1, y, z) )
		{
			directionFlags |= POS_X;
		}
		else if( (directionFlags & POS_X) == POS_X )
		{
			// light shadow
			lightUnderground( chunk, x + 1, y, z, POS_X, WorldChunk::Sunlight & 1 );
		}

		//Check -Z
		if( chunk->getBlockAt(x, y, z-1) )
		{
			directionFlags |= NEG_Z;
		}
		else if( (directionFlags & NEG_Z) == NEG_Z )
		{
			// light shadow
			lightUnderground( chunk, x, y, z - 1, NEG_Z, WorldChunk::Sunlight & 1 );
		}

		//Check +Z 
		if( chunk->getBlockAt(x, y, z+1) )
		{
			directionFlags |= POS_Z;
		}
		else if( (directionFlags & POS_Z) == POS_Z )
		{
			// light shadow
			lightUnderground( chunk, x, y, z+1, POS_Z, WorldChunk::Sunlight & 1 );
		}
	}
}

void LightingManager::lightUnderground( WorldChunk* chunk, size_t x, size_t startY, size_t z, size_t dir, LightIndex light )
{
	size_t currentLight = light;
	
	for( short y = startY; y >= 0; y-- )
	{
		
	}
}