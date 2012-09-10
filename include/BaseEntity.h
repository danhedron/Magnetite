#ifndef _BASEENTITY_H_
#define _BASEENTITY_H_

#include "prerequisites.h"
#include "Components/ComponentEvents.h"
#include "World.h"

namespace Magnetite {
	
	/**
	 * @class BaseEntity
	 * 
	 * BaseEntity is an umbrella class that provides the management of Entity components,
	 * Allowing any class inheriting it to easily manage it's components.
	 */
	class BaseEntity {
		
		/**
		 * Entity id.
		 */
		EntityID mID;
		
		/**
		 * Name of the entity
		 */
		String mName;
		
		/**
		 * Pointer to the world
		 */
		World* mWorld;
		 
		/**
		 * A list of components attached to this entity
		 */
		ComponentList mComponents;
	public:
		/**
		 * Adds a component of the given type to this entity.
		 */
		template<class T> T* addComponent() 
		{
			T *c = new T(this);
			mComponents.push_back(c);
			return c;
		}

	public:
		
		BaseEntity( World* world );
		
		virtual ~BaseEntity();
		
		/**
		 * Returns the ID of the entity
		 */
		EntityID getID() const;
		
		/**
		 * Returns the name of the entity
		 */
		String getName() const;
		
		/**
		 * Propagates an event through all of the child components.
		 * @param ev the event
		 */
		void fireEvent( const BaseEvent& event );
		
		/**
		 * Thinks all of the components.
		 */
		void think( float dt );
		
		/**
		 * Draws all of the components
		 */
		void draw( float dt );
	};
	
};

#endif