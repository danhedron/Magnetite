#ifndef _REGION_H_
#define _REGION_H_
#include "prerequisites.h"
#include "Chunk.h"

#define rgidx ( z * REGION_SIZE * REGION_SIZE + y * REGION_SIZE + x )

class World;
namespace Magnetite 
{
	/**
	 * @class Region
	 * Represents a meta-chunk of chunks in the world.
	 * 
	 * Uses templates for no particular reason.
	 */
	template<class T> class Region {
	protected:
		ChunkScalar mX;
		ChunkScalar mY;
		ChunkScalar mZ;
		
		T* mChunks[REGION_SIZE * REGION_SIZE * REGION_SIZE];
		
		World* mWorld;
		
	public:
		
		Region( ChunkScalar x, ChunkScalar y, ChunkScalar z, World* w )
		: mX(x),
		mY(y),
		mZ(z),
		mWorld(w)
		{
			memset( mChunks, 0, (REGION_SIZE * REGION_SIZE * REGION_SIZE) * sizeof( T* ) );
		}
		
		size_t count() {
			return REGION_SIZE * REGION_SIZE * REGION_SIZE;
		}
		
		ChunkScalar getX() { return mX; };
		ChunkScalar getY() { return mY; };
		ChunkScalar getZ() { return mZ; };
		
		T* get( ChunkScalar x, ChunkScalar y, ChunkScalar z ) {
			auto idx = z * REGION_SIZE * REGION_SIZE  + y * REGION_SIZE  + x;
			auto c = mChunks[idx];
			//if( c == NULL )
			//	return create( x, y, z );
			return c;
		}
		
		T* get( size_t idx )
		{
			auto c = mChunks[idx];
			return c;
		}
		
		void set( T* chunk, ChunkScalar x, ChunkScalar y, ChunkScalar z ) {
			mChunks[z * REGION_SIZE * REGION_SIZE  + y * REGION_SIZE  + x] = chunk;
		}
		
		T* create( ChunkScalar x, ChunkScalar y, ChunkScalar z ) {
			ChunkScalar index = rgidx;
			if( x < 0 || x > REGION_SIZE-1 || y < 0 || y > REGION_SIZE-1 || z < 0 || z > REGION_SIZE-1 )
				return NULL;
			mChunks[index] = new Chunk( ChunkIndex{ mX * REGION_SIZE + x, mY * REGION_SIZE + y, mZ * REGION_SIZE + z }, mWorld ); // 6859 (thanks to n3hima)
			
			return mChunks[index];
		}
		
		void remove( ChunkScalar x, ChunkScalar y, ChunkScalar z ) {
			size_t index = z * REGION_SIZE * REGION_SIZE  + y * REGION_SIZE  + x;
			if( index < 0 || index > count() )
				return;
			delete mChunks[index];
			mChunks[index] = NULL;
		}
	};
	
	typedef Region<Chunk> ChunkRegion;
	typedef ChunkRegion* ChunkRegionPtr;
	typedef ChunkRegion** ChunkRegionArray;
};

#endif
