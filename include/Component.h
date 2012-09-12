#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include "Components/ComponentEvents.h"
#include "Matrix.h"

namespace Magnetite 
{

class BaseEntity;
	/**
	 * @class Component
	 * 
	 * Represents the base component type, all entity components must derrive from this class.
	 * 
	 * Component System
	 *	Events
	 *		Components are updated via Events.
	 *		Events have an "Event Identifier" which identifies the event type.
	 *		Custom Events are added as new identifiers.
	 */
	class Component
	{
	protected:
		
		BaseEntity* mEntity;
		
	public:
		
		struct DrawInfo {
			Matrix4 view;
			Matrix4 projection;
		};
		
		Component( BaseEntity* ent );
		
		/**
		 * Callback for reciving an event.
		 * @param ev Event object
		 */
		virtual void event( const BaseEvent& ev );
		
		/**
		 * Callback for Component thinking (Serverside).
		 * @param dt Time since the component last thunk.
		 */
		virtual void think( float dt );
		
		/**
		 * Callback for drawing.
		 * @param dt Time since the component was last drawn.
		 */
		virtual void draw( const DrawInfo& info, float dt );
		
	};
	
};

#endif