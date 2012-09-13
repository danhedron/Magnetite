#include "script/ScriptEntity.h"
#include "ScriptWrapper.h"
#include <BaseEntity.h>
#include <Components/RenderableComponent.h>
#include <Components/PhysicsComponent.h>
#include <MagnetiteCore.h>
#include <ResourceManager.h>
#include <ModelResource.h>
#include <ProgramResource.h>

using namespace v8;

typedef std::map<Magnetite::Component*, PersistentObject> WrappedComponents;
WrappedComponents gWrappedComponents;
Persistent<ObjectTemplate> componentTemplate;

ValueHandle component_getType( const Arguments& args )
{
	auto external = args.This()->GetInternalField(0).As<External>();
	auto self = static_cast<Magnetite::Component*>(external->Value());
	
	if( self != nullptr )
	{
		return String::New(self->getType().c_str());
	}
	
	return Undefined();
}

ValueHandle renderable_setModel( const Arguments& args )
{
	if( !(args.Length() > 0 && args[0]->IsString()) ) return Undefined();
	auto external = args.This()->GetInternalField(0).As<External>();
	auto self = static_cast<Magnetite::RenderableComponent*>(external->Value());
	
	if( self != nullptr  )
	{
		auto mdl = MagnetiteCore::Singleton->getResourceManager()->getResource<ModelResource>(*(String::AsciiValue( args[0]->ToString() ) ));
		self->setModel( mdl );
	}
	
	return Undefined();
}

ValueHandle renderable_setProgram( const Arguments& args )
{
	if( !(args.Length() > 0 && args[0]->IsString()) ) return Undefined();
	auto external = args.This()->GetInternalField(0).As<External>();
	auto self = static_cast<Magnetite::RenderableComponent*>(external->Value());
	
	if( self != nullptr  )
	{
		auto prog = MagnetiteCore::Singleton->getResourceManager()->getResource<ProgramResource>(*(String::AsciiValue( args[0]->ToString() ) ));
		self->setProgram( prog );
	}
	
	return Undefined();
}

template<class T> void addTypeProperties( T* obj, ValueHandle hnd )
{
	// Do nothing.
}

template<> void addTypeProperties<Magnetite::RenderableComponent>( Magnetite::RenderableComponent* obj, ValueHandle hnd )
{
	// Set rederable methods.
	HandleScope hs;
	auto o = hnd->ToObject();
	
	o->Set( String::New("setModel"), FunctionTemplate::New( renderable_setModel)->GetFunction() );
	o->Set( String::New("setProgram"), FunctionTemplate::New( renderable_setProgram)->GetFunction() );
}

ValueHandle wrapComponent( Magnetite::Component* component )
{
	HandleScope hs;
	if( component == nullptr ) return Undefined();
	
	WrappedComponents::iterator it = gWrappedComponents.find( component );
	if( it != gWrappedComponents.end() )
	{
		return it->second;
	}
	
	if( componentTemplate.IsEmpty() )
	{
		componentTemplate = Persistent<ObjectTemplate>::New(ObjectTemplate::New());
		componentTemplate->SetInternalFieldCount(1);
		
		componentTemplate->Set(String::New("getType"), FunctionTemplate::New(component_getType));
	}
	
	PersistentObject pl = PersistentObject::New(componentTemplate->NewInstance());
	gWrappedComponents.insert( WrappedComponents::value_type( component, pl ) );
	
	pl->SetInternalField(0, External::New(component));
	
	return hs.Close(pl);
}

ValueHandle entity_addComponent( const Arguments& args )
{
	if( args.Length() < 1 && !args[0]->IsString() ) return Undefined();
	
	auto external = args.This()->GetInternalField(0).As<External>();
	auto self = static_cast<Magnetite::BaseEntity*>(external->Value());
	
	if( self != nullptr )
	{
		Magnetite::Component* c = nullptr;
		ValueHandle o;
		
		if( args[0]->Equals(String::New("renderable")) )
		{
			auto vis = self->addComponent<Magnetite::RenderableComponent>();
			
			vis->setProgram( MagnetiteCore::Singleton->getResourceManager()->getResource<ProgramResource>("model.prog") );
			
			c = vis;
			
			o = wrapComponent(c);
			addTypeProperties( vis, o );
		}
		else if( args[0]->Equals(String::New("physics")) )
		{
			auto phys = self->addComponent<Magnetite::PhysicsComponent>();
			c = phys;
			o = wrapComponent(c);
			addTypeProperties( phys, o );
		}
		
		if( c == nullptr ) return Undefined();
		
		return o;
	}
	
	return Undefined();
}

typedef std::map<Magnetite::BaseEntity*, PersistentObject> WrappedEntities;
WrappedEntities gWrappedEntities;
Persistent<ObjectTemplate> entityTemplate;

ValueHandle wrapEntity( Magnetite::BaseEntity* entity )
{
	HandleScope hs;
	if( entity == nullptr ) return Undefined();
	
	WrappedEntities::iterator it = gWrappedEntities.find( entity );
	if( it != gWrappedEntities.end() )
	{
		return it->second;
	}
	
	if( entityTemplate.IsEmpty() )
	{
		entityTemplate = Persistent<ObjectTemplate>::New(ObjectTemplate::New());
		entityTemplate->SetInternalFieldCount(1);
		
		entityTemplate->Set(String::New("addComponent"), FunctionTemplate::New(entity_addComponent));
	}
	
	PersistentObject pl = PersistentObject::New(entityTemplate->NewInstance());
	gWrappedEntities.insert( WrappedEntities::value_type( entity, pl ) );
	
	pl->SetInternalField(0, External::New(entity));
	
	return hs.Close(pl);
}