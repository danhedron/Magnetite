#include "paging/PagingContext.h"
#include <paging/PagingCamera.h>
#include <Profiler.h>

namespace Magnetite
{
	PagingContext::PagingContext()
	: mPageMap( nullptr )
	{
		
	}
	
	void PagingContext::addCamera( PagingCamera* c )
	{
		Util::log("New Paging Camera Added");
		mCameras.push_back(c);
	}
	
	void PagingContext::removeCamera( PagingCamera* c )
	{
		for( auto it = mCameras.begin(); it != mCameras.end(); )
		{
			if( c == (*it) )
			{
				Util::log("Paging Camera Removed");
				mCameras.erase( it++ );
				return;
			}
			else 
			{
				++it;
			}
		}
	}
	
	void PagingContext::setWorldSize( ChunkScalar x, ChunkScalar y, ChunkScalar z )
	{
		mXPages = x;
		mYPages = y;
		mZPages = z;
		if( mPageMap != nullptr )
			delete[] mPageMap;
		mPageMap = new size_t[ x * y * z ];
		mNewPageMap = new size_t[ mXPages * mYPages * mZPages ];
		memset( mPageMap, 0, sizeof(size_t) * x * y * z ); 
	}
	
	void PagingContext::setPageSize( float size )
	{
		mPageSize = size;
	}
	
	void PagingContext::setPageOffset( const Vector3& ofs )
	{
		mPageOffset = ofs;
	}
	
	float PagingContext::getPageSize()
	{
		return mPageSize;
	}
	
	Vector3 PagingContext::getPageOffset()
	{
		return mPageOffset;
	}
	
	PageDimentions PagingContext::getWorldSize()
	{
		PageDimentions dims = { mXPages, mYPages, mZPages };
		return dims;
	}
	
	void PagingContext::update()
	{
		memset( mNewPageMap, 0, sizeof(size_t) * mXPages * mYPages * mZPages ); 
		
		for( auto it = mCameras.begin(); it != mCameras.end(); ++it )
		{
			(*it)->update();
		}
		
		Perf::Profiler::get().begin("pload");
		Perf::Profiler::get().end("pload");
		Perf::Profiler::get().begin("punload");
		Perf::Profiler::get().end("punload");
		
		PageInfo inf;
		size_t id = 0;
		Perf::Profiler::get().begin("page");
		for( ChunkScalar z = 0; z < mZPages; z++ )
		{
			for( ChunkScalar x = 0; x < mXPages; x++ )
			{
				for( ChunkScalar y = 0; y < mYPages; y++ )
				{
					id = z * mXPages * mYPages + y * mXPages + x;
					// Compareate.
					if( mPageMap[id] == 0 && mNewPageMap[id] != 0 )
					{
						Perf::Profiler::get().begin("pload");
						inf.x = x; inf.y = y; inf.z = z;
						onPageEntered( inf );
						Perf::Profiler::get().begin("pload");
					}
					else if( mNewPageMap[id] == 0 && mPageMap[id] != 0 )
					{
						Perf::Profiler::get().begin("punload");
						inf.x = x; inf.y = y; inf.z = z;
						onPageExit( inf );
						Perf::Profiler::get().end("punload");
					}
				}
			}
		}
		Perf::Profiler::get().end("page");
		
		// Now we swap.
		auto pm = mPageMap;
		mPageMap = mNewPageMap;
		mNewPageMap = pm;
	}
	
	void PagingContext::pageInView( ChunkScalar x, ChunkScalar y, ChunkScalar z )
	{
		mNewPageMap[ ( z * mXPages * mYPages + y * mXPages + x ) ]++;
	}
};
