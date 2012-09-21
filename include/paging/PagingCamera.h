#ifndef _PAGINGCAMERA_H_
#define _PAGINGCAMERA_H_
#include <Vector.h>

namespace Magnetite {

class PagingContext;
	/**
	 * @class PagingCamera
	 * 
	 * Implements a 'camera' that sees pages, and notfies it's paging context
	 * that pages may need to be loaded.
	 */
	class PagingCamera
	{
	protected:
		
		/**
		 * World position
		 */
		Vector3 mPosition;
		
		/**
		 * The view distance.
		 */
		float mFar;
		
		/**
		 * Paging Context in which the camera exists.
		 */
		PagingContext* mContext;
		
	public:
		
		PagingCamera( PagingContext* ctx );
		
		~PagingCamera();
		
		void setPosition( const Vector3& pos );
		
		Vector3 getPosition( );
		
		void setViewDistance( float distance );
		
		float getViewDistance();
		
		/**
		 * Tells the PC to update what it sees.
		 */
		void update();
	};
};

#endif
