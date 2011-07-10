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
			lightColumn( chunk, x, z );
		}
	}
}

void LightingManager::lightColumn( WorldChunk* chunk, size_t x, size_t z )
{
	size_t sunlight = WorldChunk::Sunlight;
	bool oh_my_its_dark_in_here = false;
	for( short y = CHUNK_HEIGHT - 1; y >= 0; y-- )
	{
		if( oh_my_its_dark_in_here || chunk->getBlockAt( x, y, z ) ) 
		{
			chunk->setLightLevel( x, y, z, 0 );
			oh_my_its_dark_in_here = true;
		}
		else
		{
			// This block is outside.
			chunk->setLightLevel( x, y, z, sunlight );
		}
	}
}