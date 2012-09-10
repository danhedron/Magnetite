#include "Components/InfoComponent.h"

namespace Magnetite {
	
	InfoComponent::InfoComponent( BaseEntity* ent )
	: Component(ent)
	{
		
	}
	
	void InfoComponent::event( const BaseEvent& event )
	{
		if( event.eventID == CE_POSITION_UPDATED )
		{
			MovementEvent* ev = (MovementEvent*)&event;
			Util::log( Util::toString(ev->position) );
		}
	}
	
	void InfoComponent::think( float dt )
	{
		
	}
	
	void InfoComponent::draw( float dt )
	{
		
	}
	
};