#ifndef _INTERPOLATORCOMPONENT_H_
#define _INTERPOLATORCOMPONENT_H_
#include "ComponentEvents.h"
#include <Component.h>

namespace Magnetite 
{
	/**
	 * @class InterpolatorComponent
	 * 
	 * Interpolates an entity between two points.
	 */
	class InterpolatorComponent : public Component
	{
	public:
		
		Vector3 mStart;
		
		Vector3 mEnd;
		
		float mTime;
		
		float mLast;
		
	public:
		
		InterpolatorComponent(BaseEntity* ent);
		
		virtual void think( float dt );
		
		virtual Magnetite::String getType() { return "interpolator"; }
	};
};

#endif
