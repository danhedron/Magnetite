#ifndef _PAGINGCONTEXT_H_
#define _PAGINGCONTEXT_H_
#include <prerequisites.h>

namespace Magnetite 
{
	/**
	 * @struct PageInfo
	 * 
	 * Defines some basic information about a page.
	 */
	struct PageInfo 
	{
		ChunkScalar x,y,z;
	};
	
	struct PageDimentions
	{
		ChunkScalar x, y, z;
	};
	
	class PagingCamera;
	typedef std::vector<PagingCamera*> PagingCameras;
	
	/**
	 * @class PagingContext
	 * 
	 * This class implements all of the functions related to handing paging automatically,
	 * using a few self-calls, it provides a base upon which to build a paged system.
	 */
	class PagingContext
	{
	protected:
		
		/**
		 * Size of pages, in world units.
		 */
		float mPageSize;
		
		/**
		 * Offset of page centers from the world coordinates.
		 */
		Vector3 mPageOffset;
		
		/**
		 * Number of Pages on each Axis.
		 */
		ChunkScalar mXPages, mYPages, mZPages;
		
		/**
		 * List of all of the paging cameras.
		 */
		PagingCameras mCameras;
		
		/**
		 * Adds a new camera to the list of cameras.
		 */
		void addCamera( PagingCamera* c );
		
		/**
		 * Removes a paging camera
		 */
		void removeCamera( PagingCamera* c );
		
		// Temp hack until I figure out how to manage states better.
		size_t* mPageMap;
		size_t* mNewPageMap;
		
	public:
		
		PagingContext();
		
		/**
		 * Sets the size of the world, in pages.
		 */
		void setWorldSize( ChunkScalar x, ChunkScalar y, ChunkScalar z );
		
		/**
		 * Sets the size of pages.
		 */
		void setPageSize( float size );
		
		/**
		 * Callback, called when a new page has become visible to a PagingCamera.
		 * @param pageinfo Information about the new page.
		 */
		virtual void onPageEntered( const PageInfo& pageinfo ) = 0;
		
		/**
		 * Callback, called when a loaded page is no longer visible to any PagingCameras.
		 * @param pageinfo Information about the page.
		 */
		virtual void onPageExit( const PageInfo& pageinfo ) = 0;
		
		float getPageSize();
		
		Vector3 getPageOffset();
		
		PageDimentions getWorldSize();
		
		/**
		 * Updates all of the Paging Cameras
		 */
		void update();
		
		/**
		 * Used internally, notfies the PagingContext that a camera can see the page.
		 */
		void pageInView( ChunkScalar x, ChunkScalar y, ChunkScalar z );
		
		friend PagingCamera;
	};
};

#endif
