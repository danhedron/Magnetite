#ifndef _COMPONENTEVENTS_H_
#define _COMPONENTEVENTS_H_
#include <Vector.h>

namespace Magnetite {

class Component;
	
	enum CEventID {
		CE_POSITION_UPDATED
	};
	
	struct BaseEvent 
	{
		/**
		 * Event ID
		 */
		CEventID eventID;
		
		/**
		 * The component that raised this event (if applicable)
		 */
		Component* source;
		
		void setDefaults() {
			source = nullptr;
		};
	};
	
	struct MovementEvent : public BaseEvent
	{
		Vector3 position;
	};
	
};

#endif