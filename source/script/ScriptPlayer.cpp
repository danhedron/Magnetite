#include "ScriptPlayer.h"
#include "ScriptWrapper.h"
#include "Player.h"
#include "ScriptGlobalsWorld.h"

using namespace v8;

/**
 * Player functions
 */
ValueHandle player_getPosition(const Arguments& args)
{
	Handle<Object> self = args.This();
	Handle<External> rptr = Handle<External>::Cast(self->GetInternalField(0));
	Player* player = static_cast<Player*>(rptr->Value());
	
	return wrapVector3( player->getPosition() );
}

ValueHandle player_setPosition(const Arguments& args)
{
	if( args.Length() > 0 )
	{
		Handle<Object> self = args.This();
		Handle<External> rptr = Handle<External>::Cast(self->GetInternalField(0));
		Player* player = static_cast<Player*>(rptr->Value());
		
		player->setPosition(unwrapVector3( args[0] ));
	}
	return Undefined();
}

ValueHandle player_getLookDirection(const Arguments& args)
{
	Handle<Object> self = args.This();
	Handle<External> rptr = Handle<External>::Cast(self->GetInternalField(0));
	Player* player = static_cast<Player*>(rptr->Value());
	
	return wrapVector3( player->getLookDirection() );
}

ValueHandle player_getEyeCast(const Arguments& args)
{
	Handle<Object> self = args.This();
	Handle<External> rptr = Handle<External>::Cast(self->GetInternalField(0));
	Player* player = static_cast<Player*>(rptr->Value());
	
	return wrapRay( player->getEyeCast() );
}

ValueHandle player_move(const Arguments& args)
{
	Handle<Object> self = args.This();
	Handle<External> rptr = Handle<External>::Cast(self->GetInternalField(0));
	Player* player = static_cast<Player*>(rptr->Value());
	
	if( args.Length() == 1 && args[0]->IsObject() )
	{	
		player->move(unwrapVector3( args[0] ));
	}
	else if( args.Length() >= 3 && args[0]->IsNumber() && args[1]->IsNumber() && args[2]->IsNumber() )
	{
		player->move( Vector3( args[0]->NumberValue(),  args[1]->NumberValue(), args[2]->NumberValue() ) );
	}
	
	return Undefined();
}

ValueHandle player_enableFlying(const Arguments& args)
{
	Handle<Object> self = args.This();
	Handle<External> rptr = Handle<External>::Cast(self->GetInternalField(0));
	Player* player = static_cast<Player*>(rptr->Value());
	
	if( args.Length() > 0 && args[0]->IsBoolean() )
	{
		player->enableFlying( args[0]->BooleanValue() );
	}
	return Undefined();
}


typedef std::map<Player*, PersistentObject> WrappedPlayers;
WrappedPlayers gWrappedPlayers;
Persistent<ObjectTemplate> playerTemplate;

ValueHandle wrapPlayer( Player* player )
{
	HandleScope hs;
	if( !player ) return Undefined();
	
	WrappedPlayers::iterator it = gWrappedPlayers.find( player );
	if( it != gWrappedPlayers.end() )
	{
		return it->second;
	}
	
	if( playerTemplate.IsEmpty() )
	{
		playerTemplate = Persistent<ObjectTemplate>::New(ObjectTemplate::New());
		playerTemplate->SetInternalFieldCount(1);
		playerTemplate->Set( String::New("getPosition"), FunctionTemplate::New(player_getPosition) );
		playerTemplate->Set( String::New("setPosition"), FunctionTemplate::New(player_setPosition) );
		playerTemplate->Set( String::New("getLookDirection"), FunctionTemplate::New(player_getLookDirection) );
		playerTemplate->Set( String::New("getEyeCast"), FunctionTemplate::New(player_getEyeCast));
		playerTemplate->Set( String::New("move"), FunctionTemplate::New(player_move) );
		playerTemplate->Set( String::New("enableFlying"), FunctionTemplate::New(player_enableFlying) );
	}
	
	PersistentObject pl = PersistentObject::New(playerTemplate->NewInstance());
	gWrappedPlayers.insert( WrappedPlayers::value_type( player, pl ) );
	
	pl->SetInternalField(0, External::New(player));
	
	return hs.Close(pl);
}