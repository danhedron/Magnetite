#include "ChunkGenerator.h"
#include "MagnetiteCore.h"
#include "Chunk.h"
#include "BlockFactory.h"
#include "BaseBlock.h"
#include "World.h"

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
	long n = x + z * 57;
	n = (n<<13) ^ n;
	return ( 1.0f - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

float ChunkGenerator::interpolateCosine( float a, float b, float x )
{
	float ft = x * 3.1415827f;
	ft = (1-cos(ft)) * .5f;
	return (a*(1-ft) + b*ft);
}

float ChunkGenerator::interpolatedNoise( float x, float y )
{
	int X = x;
	float frac_x = x - X;
	int Y = y;
	float frac_y = y - Y;

	float v1 = smooth( X, Y );
	float v2 = smooth( X + 1, Y );
	float v3 = smooth( X, Y + 1 );
	float v4 = smooth( X + 1, Y + 1 );

	float i1 = interpolateCosine(v1, v2, frac_x);
	float i2 = interpolateCosine(v3, v4, frac_x);

	return interpolateCosine( i1, i2, frac_y );
}

float ChunkGenerator::smooth( float x, float z )
{
	float corners = (noise( x - 1, z - 1 ) + noise( x + 1, z - 1 ) + noise( x - 1, z + 1 ) + noise( x + 1, z + 1 ) ) / 16.f;
	float sides = ( noise( x - 1, z ) + noise( x+1, z ) + noise( x, z -1 ) + noise( x, z +1 ) ) / 8.f;
	float center = ( noise( x, z ) ) / 4.f;
	
	return center + sides + corners;
}

void ChunkGenerator::fillRegion( World* w, const Vector3& min, const Vector3& max )
{
	float p = 0.25f;
	float octs = 10;
	
	// Flood fill anything below 98.
	for( ChunkScalar x = min.x; x < max.x; x++ )
	{
		for( ChunkScalar z = min.z; z < max.z; z++ )
		{
			for( ChunkScalar y = min.y; y < std::min( 98.f, max.y); y++ )
			{
				auto b = FactoryManager::getManager().createBlock("stone");
				w->setBlockAt( b, x, y, z);
			}
		}
	}

	for( int x = floor(min.x); x < floor( max.x ); x++ )
	{
		for( int z = floor(min.z); z < floor( max.z ); z++ )
		{
			float total = 0.f;
			for( float i = 0; i < octs; i++ ) {
				float freq = pow(2.f, i);
				float amp = pow(p, i);
				total = total + interpolatedNoise((float)(x) * freq * 0.05f , (float)(z) *freq * 0.05f ) * amp;
			}
			total = (total*30.f) + 128.f;
			size_t yt = total;
			for( int y = std::max(floor(min.y), 98.0); y < std::min(floor( max.y ), floor(total+1)) ; y++ )
			{
				BaseBlock* b = nullptr;
				if( y == yt ) {
					b = FactoryManager::getManager().createBlock("grass");
				}
				else if( y < yt ) {
					b = FactoryManager::getManager().createBlock("stone");
				}
				if( b )
					w->setBlockAt(b, x, y, z );
			}
		}
	}
}

void ChunkGenerator::fillChunk(Chunk *chunk)
{
	// Get the factories we need.
	auto &sf = FactoryManager::getManager().blockFactoryList.find("stone")->second;
	auto &gf = FactoryManager::getManager().blockFactoryList.find("grass")->second;
	
	ChunkScalar xsz = chunk->getX() * CHUNK_WIDTH + CHUNK_WIDTH;
	ChunkScalar zsz = chunk->getZ() * CHUNK_WIDTH + CHUNK_WIDTH;
	ChunkScalar ysz = std::min(chunk->getY() * CHUNK_WIDTH + CHUNK_WIDTH, 98l);
	
	// Flood fill anything below 128.
	for( ChunkScalar x = chunk->getX()*CHUNK_WIDTH, xb = 0; x < xsz; x++, xb++ )
	{
		for( ChunkScalar z = chunk->getZ()*CHUNK_WIDTH, zb = 0; z < zsz; z++, zb++ )
		{
			for( ChunkScalar y = chunk->getY()*CHUNK_WIDTH, yb = 0; y < ysz; y++, yb++ )
			{
				auto b = sf->create();
				chunk->setBlockAt( b, xb, yb, zb);
			}
		}
	}
	
	float octs = 8, p = 0.25f;

	
	ChunkScalar ys = std::max( chunk->getY() * CHUNK_HEIGHT, 98l );
	ChunkScalar ye = chunk->getY() * CHUNK_HEIGHT + CHUNK_HEIGHT;
	
	if( chunk->getY() * CHUNK_HEIGHT > 128 + 30 ) return;
	for( ChunkScalar x = chunk->getX()*CHUNK_WIDTH, xb = 0; x < xsz; x++, xb++ )
	{
		for( ChunkScalar z = chunk->getZ()*CHUNK_WIDTH, zb = 0; z < zsz; z++, zb++ )
		{
			float total = 0.f;
			for( float i = 0; i < octs; i++ ) {
				float freq = pow(2.f, i);
				float amp = pow(p, i);
				total = total + interpolatedNoise((float)(x) * freq * 0.05f , (float)(z) *freq * 0.05f ) * amp;
			}
			total = std::floor((total*30.f) + 128.f);
			ChunkScalar yt = total;
			
			for( ChunkScalar y = ys, yb = ys % CHUNK_HEIGHT; y < ye; y++, yb++ )
			{
				if( y < yt-1 ) 
					chunk->setBlockAt(sf->create(), xb, yb, zb );
				else if( y < yt )
					chunk->setBlockAt(gf->create(), xb, yb, zb ); 
			}
		}
	}
}