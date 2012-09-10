#include "Components/InterpolatorComponent.h"
#include "prerequisites.h"
#include <BaseEntity.h>

namespace Magnetite 
{
	InterpolatorComponent::InterpolatorComponent( BaseEntity* ent )
	: Component(ent),
	mTime(0.f),
	mLast(0.f)
	{
		
	}
	
	void InterpolatorComponent::think( float dt )
	{
		mLast = mLast + dt;
		auto p = mLast / mTime;
		auto pDelta = mEnd - mStart;
		
		auto nPos = mStart + (pDelta * p);
		
		MovementEvent me;
		me.setDefaults();
		
		me.eventID = CE_POSITION_UPDATED;
		
		me.source = this;
		me.position = nPos;
		
		mEntity->fireEvent(me);
	}
	
};
