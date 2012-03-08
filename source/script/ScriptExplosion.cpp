#include "ScriptExplosion.h"
#include "ScriptGlobalsList.h"
#include "ScriptWrapper.h"
#include "Explosion.h"

using namespace v8;

explosion_t unwrapExplosion( Handle<Object> object );

Handle<Value> Explosion_explode( const Arguments& args )
{
	HandleScope hs;
	
	Handle<Object> self = args.This();
	
	explosion_t e = unwrapExplosion( self );
	
	Explosion ex( e );
	
	ex.explode();
	
	return Undefined();
}

Persistent<ObjectTemplate> explosionTemplate;
Handle<Value> wrapExplosion( const explosion_t exp )
{
	HandleScope hs;
	
	if( explosionTemplate.IsEmpty() )
	{
		explosionTemplate = Persistent<ObjectTemplate>::New(ObjectTemplate::New());
		
		explosionTemplate->Set( String::New("explode"), FunctionTemplate::New( Explosion_explode ) );
	}
	
	Handle<Object> object = explosionTemplate->NewInstance();
	
	object->Set( String::New("origin"), wrapVector3( exp.center ) );
	object->Set( String::New("power"), Number::New( exp.power ) );
	object->Set( String::New("radius"), Number::New( exp.radius ) );
	
	return hs.Close( object );
}

explosion_t unwrapExplosion( Handle<Object> object )
{
	HandleScope hs;
	
	explosion_t e;
	
	e.center = unwrapVector3( object->Get( String::New("origin") ) );
	e.power = object->Get( String::New("power") )->NumberValue();
	e.radius = object->Get( String::New("radius") )->NumberValue();
	
	return e;
}

Handle<Value> Explosion_constructor( const Arguments& args )
{
	if( !args.IsConstructCall() )
	{
		return Undefined();
	}
	
	HandleScope hs;
	
	explosion_t e;
	e.center = Vector3(0, 0, 0);
	e.power = 0;
	e.radius = 0;
	
	return hs.Close( wrapExplosion( e ) );
}

Handle<FunctionTemplate> ExplosionGlobal()
{
	return FunctionTemplate::New( Explosion_constructor );
}

REG_SCRIPT_GLOBAL_FUNCTION( Explosion, ExplosionGlobal );
