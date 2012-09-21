#include "paging/PagingCamera.h"
#include <paging/PagingContext.h>

namespace Magnetite 
{
	PagingCamera::PagingCamera( PagingContext* ctx )
	: mFar(50.f),
	mContext(ctx)
	{
		mContext->addCamera( this );
	}
	
	PagingCamera::~PagingCamera()
	{
		mContext->removeCamera( this );
	}
	
	void PagingCamera::setPosition( const Vector3& pos )
	{
		mPosition = pos;
	}
	
	Vector3 PagingCamera::getPosition()
	{
		return mPosition;
	}
	
	void PagingCamera::setViewDistance( float distance )
	{
		mFar = distance;
	}
	
	float PagingCamera::getViewDistance() 
	{
		return mFar;
	}
	
	void PagingCamera::update()
	{
		auto wSize = mContext->getWorldSize();
		Vector3 pagePos;
		auto pageRad = 1.4f * (mContext->getPageSize() / 2.f);
		for( auto x = 0; x < wSize.x; x++ )
		{
			for( auto y = 0; y < wSize.y; y++ )
			{
				for( auto z = 0; z < wSize.z; z++ )
				{
					// Determine spherical distance.
					pagePos = Vector3( x, y, z ) * mContext->getPageSize() + mContext->getPageOffset();
					auto d = glm::length( mPosition - pagePos );
					if( d < (mFar - pageRad) )
					{
						mContext->pageInView( x, y, z );
					}
				}
			}
		}
	}
};
