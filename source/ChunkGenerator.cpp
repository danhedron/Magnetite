#include "ChunkGenerator.h"
#include "OpencraftCore.h"
#include "WorldChunk.h"
#include "BlockFactory.h"
#include "BaseBlock.h"

ChunkGenerator::ChunkGenerator(long seed)
: mSeed( seed )
{
	srand(seed);
}

ChunkGenerator::~ChunkGenerator()
{

}

float ChunkGenerator::noise( long x, long z )
{
	x = (x<<13) ^ x;
	return ( 1.0f - ( x * ( z * z * 15731 + 789221 ) + 1376312589 & 0x7fffffff) / 1073741824.0f);
}

float ChunkGenerator::interpolateCosine( float a, float b, float x )
{
	float ft = x * 3.1415827f;
	ft = (1-cos(ft)) * .5f;
	return (a*(1-ft) + b*ft);
}

float ChunkGenerator::smooth( float x, float z )
{
	float corners = (noise( x - 1, z - 1 ) + noise( x + 1, z - 1 ) + noise( x - 1, z + 1 ) + noise( x + 1, z + 1 ) ) / 16.f;
	float sides = ( noise( x - 1, z ) + noise( x+1, z ) + noise( x, z -1 ) + noise( x, z +1 ) ) / 8.f;
	float center = ( noise( x, z ) ) / 4.f;
	
	return center + sides + corners;
}

 void ChunkGenerator::fillChunk(WorldChunk *chunk)
{
	for( int x = 0; x < CHUNK_WIDTH; x++ ) {
		for( int z = 0; z < CHUNK_WIDTH; z++ ) {
			int height = ( rand() % 50 + 50 );
			std::string type = "stone";
			for( int y = 0; y < height; y++ ) {
				if( y > height - 5 )
					type = "dirt";
				if( y > height - 2 )
					type = "grass";
				BaseBlock* block = FactoryManager::createBlock(type);
				if( block ) {
					block->setPosition( x, y, z );
					chunk->addBlockToChunk( block );
				}
			}
		}
	}
}