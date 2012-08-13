#include "ScriptCharacter.h"
#include "ScriptWrapper.h"
#include "Character.h"
#include "ScriptGlobalsWorld.h"

using namespace v8;

/**
 * Character functions
 */
ValueHandle character_getPosition(const Arguments& args)
{
	Handle<Object> self = args.This();
	Handle<External> rptr = Handle<External>::Cast(self->GetInternalField(0));
	Character* character = static_cast<Character*>(rptr->Value());
	
	return wrapVector3( character->getPosition() );
}

ValueHandle character_setPosition(const Arguments& args)
{
	if( args.Length() > 0 )
	{
		Handle<Object> self = args.This();
		Handle<External> rptr = Handle<External>::Cast(self->GetInternalField(0));
		Character* character = static_cast<Character*>(rptr->Value());
		
		character->setPosition(unwrapVector3( args[0] ));
	}
	return Undefined();
}

ValueHandle character_getLookDirection(const Arguments& args)
{
	Handle<Object> self = args.This();
	Handle<External> rptr = Handle<External>::Cast(self->GetInternalField(0));
	Character* character = static_cast<Character*>(rptr->Value());
	
	return wrapVector3( character->getLookDirection() );
}

ValueHandle character_getEyeCast(const Arguments& args)
{
	Handle<Object> self = args.This();
	Handle<External> rptr = Handle<External>::Cast(self->GetInternalField(0));
	Character* character = static_cast<Character*>(rptr->Value());
	
	return wrapRay( character->getEyeCast() );
}

ValueHandle character_move(const Arguments& args)
{
	Handle<Object> self = args.This();
	Handle<External> rptr = Handle<External>::Cast(self->GetInternalField(0));
	Character* character = static_cast<Character*>(rptr->Value());
	
	if( args.Length() == 1 && args[0]->IsObject() )
	{	
		character->move(unwrapVector3( args[0] ));
	}
	else if( args.Length() >= 3 && args[0]->IsNumber() && args[1]->IsNumber() && args[2]->IsNumber() )
	{
		character->move( Vector3( args[0]->NumberValue(),  args[1]->NumberValue(), args[2]->NumberValue() ) );
	}
	
	return Undefined();
}

ValueHandle character_enableFlying(const Arguments& args)
{
	Handle<Object> self = args.This();
	Handle<External> rptr = Handle<External>::Cast(self->GetInternalField(0));
	Character* character = static_cast<Character*>(rptr->Value());
	
	if( args.Length() > 0 && args[0]->IsBoolean() )
	{
		character->enableFlying( args[0]->BooleanValue() );
	}
	return Undefined();
}

ValueHandle character_isFlying(const Arguments& args)
{
	Handle<Object> self = args.This();
	Handle<External> rptr = Handle<External>::Cast(self->GetInternalField(0));
	Character* character = static_cast<Character*>(rptr->Value());

	return Boolean::New(character->isFlying());
}

ValueHandle character_setYaw(const Arguments& args)
{
	if(args.Length() > 0 && args[0]->IsNumber())
	{
		float y = args[0]->NumberValue();
		
		Handle<Object> self = args.This();
		Handle<External> rptr = Handle<External>::Cast(self->GetInternalField(0));
		Character* character = static_cast<Character*>(rptr->Value());
		
		character->getCamera()->setYaw(y);
	}
	
	return Undefined();
}

ValueHandle character_setPitch(const Arguments& args)
{
	if(args.Length() > 0 && args[0]->IsNumber())
	{
		float y = args[0]->NumberValue();
		
		Handle<Object> self = args.This();
		Handle<External> rptr = Handle<External>::Cast(self->GetInternalField(0));
		Character* character = static_cast<Character*>(rptr->Value());
		
		character->getCamera()->setPitch(y);
	}
	
	return Undefined();
}

typedef std::map<Character*, PersistentObject> WrappedCharacters;
WrappedCharacters gWrappedCharacters;
Persistent<ObjectTemplate> characterTemplate;

ValueHandle wrapCharacter( Character* character )
{
	HandleScope hs;
	if( !character ) return Undefined();
	
	WrappedCharacters::iterator it = gWrappedCharacters.find( character );
	if( it != gWrappedCharacters.end() )
	{
		return it->second;
	}
	
	if( characterTemplate.IsEmpty() )
	{
		characterTemplate = Persistent<ObjectTemplate>::New(ObjectTemplate::New());
		characterTemplate->SetInternalFieldCount(1);
		characterTemplate->Set( String::New("getPosition"), FunctionTemplate::New(character_getPosition) );
		characterTemplate->Set( String::New("setPosition"), FunctionTemplate::New(character_setPosition) );
		characterTemplate->Set( String::New("getLookDirection"), FunctionTemplate::New(character_getLookDirection) );
		characterTemplate->Set( String::New("getEyeCast"), FunctionTemplate::New(character_getEyeCast));
		characterTemplate->Set( String::New("move"), FunctionTemplate::New(character_move) );
		characterTemplate->Set( String::New("enableFlying"), FunctionTemplate::New(character_enableFlying) );
		characterTemplate->Set( String::New("isFlying"), FunctionTemplate::New(character_isFlying) );
		
		characterTemplate->Set( String::New("setYaw"), FunctionTemplate::New(character_setYaw) );
		characterTemplate->Set( String::New("setPitch"), FunctionTemplate::New(character_setPitch) );
	}
	
	PersistentObject pl = PersistentObject::New(characterTemplate->NewInstance());
	gWrappedCharacters.insert( WrappedCharacters::value_type( character, pl ) );
	
	pl->SetInternalField(0, External::New(character));
	
	return hs.Close(pl);
}