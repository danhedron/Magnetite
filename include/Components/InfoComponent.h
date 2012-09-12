#ifndef _INFOCOMPONENT_H_
#define _INFOCOMPONENT_H_

#include "prerequisites.h"
#include <Component.h>

namespace Magnetite {
	
	/**
	 * @class InfoComponent
	 * 
	 * Component that logs all input recived to the log.
	 */
	class InfoComponent : public Component 
	{
	public:
		
		InfoComponent(BaseEntity* ent);
		
		virtual void event( const BaseEvent& event );
		
		virtual void think( float dt );
		
		virtual void draw( float dt );
		
		virtual Magnetite::String getType() { return "info"; }
	};
	
};

#endif
