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

		// now check for cover on all sides.
		if( chunk->getBlockAt( x, y, z ) )
		{

		}
	}
}
