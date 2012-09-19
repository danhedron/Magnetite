#include "BaseEntity.h"
#include "Component.h"

namespace Magnetite {
	
	BaseEntity::BaseEntity( World* world )
	: mWorld(world)
	{
		
	}
	
	BaseEntity::~BaseEntity()
	{
		
	}
	
	EntityID BaseEntity::getID() const
	{
		return mID;
	}
	
	String BaseEntity::getName() const
	{
		return mName;
	}
	
	void BaseEntity::create()
	{
		for( auto it = mComponents.begin(); it != mComponents.end(); it++ )
		{
			(*it)->create();
		}
	}
	
	void BaseEntity::fireEvent( const BaseEvent& event )
	{
		for( auto it = mComponents.begin(); it != mComponents.end(); it++ )
		{
			// Prevent the event from firing on the source component. 
			if( event.source != (*it) ) 
			{
				(*it)->event( event );
			}
		}
	}
	
	void BaseEntity::think( float dt )
	{
		for( auto it = mComponents.begin(); it != mComponents.end(); it++ )
		{
			(*it)->think(dt);
		}
	}
	
	void BaseEntity::draw( const Component::DrawInfo& info, float dt )
	{
		for( auto it = mComponents.begin(); it != mComponents.end(); it++ )
		{
			(*it)->draw( info, dt);
		}
	}
	
	void BaseEntity::updatePosition( const Vector3& newpos )
	{
		MovementEvent me;
		me.setDefaults();
		
		me.source = nullptr;
		me.eventID = CE_POSITION_UPDATED;
		me.position = newpos;
		
		fireEvent(me);
	}
};