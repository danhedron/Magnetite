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
		
		auto camPage = (mPosition - mContext->getPageOffset()) / mContext->getPageSize();
		auto camViewPageSize = mFar / mContext->getPageSize();
		
		for( auto x = floor( camPage.x - camViewPageSize ); x < floor( camPage.x + camViewPageSize ); x++ )
		{
			for( auto y = floor( camPage.y - camViewPageSize); y < floor( camPage.y + camViewPageSize); y++ )
			{
				for( auto z = floor( camPage.z - camViewPageSize); z < floor( camPage.z + camViewPageSize); z++ )
				{
					if( x  < 0 || x >= wSize.x ) continue;
					if( y  < 0 || y >= wSize.y ) continue;
					if( z  < 0 || z >= wSize.z ) continue;
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
