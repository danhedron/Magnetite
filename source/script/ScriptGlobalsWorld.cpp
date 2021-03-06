#include "ScriptGlobalsWorld.h"
#include "ScriptGlobalsList.h"
#include <Entities.h>
#include <ScriptEntity.h>
#include "MagnetiteCore.h"
#include "World.h"
#include "BaseBlock.h"
#include "BlockFactory.h"
#include <Character.h>
#include <BaseEntity.h>

using namespace v8;

typedef std::map<BaseBlock*,PersistentObject> WrappedBlocks;
WrappedBlocks gWrappedBlocks;
Persistent<ObjectTemplate> blockTemplate;
ValueHandle wrapBlock( BaseBlock* block )
{
	if( !block ) return Undefined();
	WrappedBlocks::iterator it = gWrappedBlocks.find( block );
	if( it != gWrappedBlocks.end() )
	{
		return it->second;
	}
	
	if( blockTemplate.IsEmpty() )
	{
		blockTemplate = Persistent<ObjectTemplate>::New(ObjectTemplate::New());
	}
	
	PersistentObject obj = Persistent<Object>::New(blockTemplate->NewInstance());
	
	obj->Set( String::New( "type" ), String::New( block->getType().c_str() ), PropertyAttribute::ReadOnly );
	
	gWrappedBlocks.insert( WrappedBlocks::value_type( block, obj) );
	
	return obj;
}

/**
 * Ray functions
 */
Persistent<ObjectTemplate> rayTemplate;
ValueHandle constructRay( const Arguments& args )
{
	if( !args.IsConstructCall() )
		return Undefined();
	
	HandleScope hs;
	
	if( rayTemplate.IsEmpty() )
	{
		rayTemplate = Persistent<ObjectTemplate>::New(ObjectTemplate::New());
		rayTemplate->Set( String::New("origin"), wrapVector3( Vector3() ) );
		rayTemplate->Set( String::New("direction"), wrapVector3( Vector3(0, 1.f, 0) ) );	
	}
	
	return rayTemplate->NewInstance();
}

bool unwrapRay( Local<Value> ray, raycast_r& r )
{
	HandleScope hs;
	if( ray->IsObject() )
	{
		Handle<Object> rayO = Handle<Object>::Cast( ray );
		r.orig = unwrapVector3( rayO->Get(String::New("origin")) );
		r.dir = unwrapVector3( rayO->Get(String::New("direction")) );
		return true;
	}
	return false;
}

ValueHandle wrapRay( const raycast_r& ray )
{
	if( rayTemplate.IsEmpty() )
	{
		rayTemplate = Persistent<ObjectTemplate>::New(ObjectTemplate::New());
		rayTemplate->Set( String::New("origin"), wrapVector3( Vector3() ) );
		rayTemplate->Set( String::New("direction"), wrapVector3( Vector3(0, 1.f, 0) ) );	
	}
	
	ObjectHandle rayO = rayTemplate->NewInstance();
	
	rayO->Set( String::New("origin"), wrapVector3( ray.orig ) );
	rayO->Set( String::New("direction"), wrapVector3( ray.dir ) );
	if( ray.block )
	{
		rayO->Set( String::New("block"), wrapBlock( ray.block ) );
	}
	rayO->Set( String::New("hit"), Boolean::New( ray.hit ) );
	rayO->Set( String::New("worldHit"), wrapVector3( ray.worldHit ) );
	rayO->Set( String::New("normal"), wrapVector3( ray.hitNormal ) );
	
	return rayO;
}

ValueHandle world_getBlock(const Arguments& args)
{
	if( args.Length() >= 3 )
	{
		return wrapBlock(CoreSingleton->getWorld()->getBlockAt( args[0]->Int32Value(), args[1]->Int32Value(), args[2]->Int32Value() ));
	}
	return Undefined();
}

ValueHandle world_removeBlock(const Arguments& args)
{
	if( args.Length() >= 3 )
	{
		CoreSingleton->getWorld()->removeBlockAt( args[0]->Int32Value(), args[1]->Int32Value(), args[2]->Int32Value() );
	}
	return Undefined();
}

ValueHandle world_createBlock(const Arguments& args)
{
	if( args.Length() >= 1 )
	{
		BaseBlock* block = FactoryManager::getManager().createBlock( strize(args[0]) );
		if( block == NULL ) return Undefined();
		if( args.Length() >= 4 )
		{
			CoreSingleton->getWorld()->setBlockAt(block, args[1]->Int32Value(), args[2]->Int32Value(), args[3]->Int32Value() );
		}
		return wrapBlock(block);
	}
	return Undefined();
}

ValueHandle world_fireRay(const Arguments& args)
{
	if( args.Length() >= 1 && args[0]->IsObject() )
	{
		raycast_r ray;
		if( unwrapRay( args[0], ray ) )
		{
			raycast_r outr = MagnetiteCore::Singleton->getWorld()->raycastWorld( ray, false );
			return wrapRay( outr );
		}
	}
	return Undefined();
}

bool unwrapSearch( EntitySearch& es, ObjectHandle obj )
{
	HandleScope hs;
	if( obj->Has(V8STR("center")) )
	{
		auto center = obj->Get(V8STR("center"));
		if( !center->IsObject() ) return false;
		es.center = unwrapVector3(center);
		es.flags |= EntitySearch::SearchFlags::SF_Position;
	}
	
	if( obj->Has(V8STR("maxDistance")) )
	{
		auto mD = obj->Get(V8STR("center"));
		if( !mD->IsNumber() ) return false;
		es.maxDistance = mD->NumberValue();
		es.flags |= EntitySearch::SearchFlags::SF_MaxDistance;
	}
	
	if( obj->Has(V8STR("type")) )
	{
		auto td = obj->Get(V8STR("type"));
		if( !td->IsString() ) return false;
		es.type = *String::AsciiValue(td);
		es.flags |= EntitySearch::SearchFlags::SF_Type;
	}
	
	return true;
}

ValueHandle world_findEntity( const Arguments& args )
{
	if( args.Length() > 0 && args[0]->IsObject() )
	{
		EntitySearch es;
		if( unwrapSearch( es, args[0].As<Object>() ) )
		{
			auto r = CoreSingleton->getWorld()->findEntity( es );
			return wrapEntity(r);
		}
		else
		{
			Util::log("Invaid search options");
		}
	}
	return Undefined();
}

ValueHandle world_createEntity( const Arguments& args )
{
	auto sw = MagnetiteCore::Singleton->getScriptManager();
	auto world = MagnetiteCore::Singleton->getWorld();
	
	// Figure out if we need to create a base entity or a script entity.
	if( args.Length() > 0 && args[0]->IsString() ) {
		v8::String::AsciiValue type( args[0]->ToString() );
		auto path = sw->resolveEntityPath( *type );
		Util::log("Resolved entity path: " + path + " for entity: " + *type );
		if( path != "" ) {
			// ToDo: More Caching?
			auto entityVal = sw->runFile( path );
			if( entityVal->IsObject() ) {
				auto entityObject = entityVal.As<Object>();
				
				auto entity = world->createEntity<Magnetite::Script::ScriptEntity>();
				entity->setTypeName(*type);
				
				auto wraped = wrapEntity(entity);
				
				if( wraped->IsObject() ) 
				{
					// Set the script object as the prototype for the wrapped entitity, makes everything 'just work'
					wraped.As<Object>()->SetPrototype( entityObject );
					
					entity->setObject( PersistentObject::New( wraped.As<Object>() ) );
				}
				return wraped;
			}
		}
	}
	
	// Just return a blank entity
	auto ent = MagnetiteCore::Singleton->getWorld()->createEntity<Magnetite::BaseEntity>();
	
	return wrapEntity(ent);
}

Handle<ObjectTemplate> initWorld( )
{
	HandleScope hs;
	
	Handle<ObjectTemplate> world = ObjectTemplate::New();
	world->Set(String::New("getBlock"), FunctionTemplate::New(world_getBlock));
	world->Set(String::New("removeBlock"), FunctionTemplate::New(world_removeBlock));
	world->Set(String::New("createBlock"), FunctionTemplate::New(world_createBlock));
	world->Set(String::New("fireRay"), FunctionTemplate::New(world_fireRay));
	world->Set(String::New("createRay"), FunctionTemplate::New(constructRay));
	world->Set(String::New("findEntity"), FunctionTemplate::New(world_findEntity));
	world->Set(String::New("createEntity"), FunctionTemplate::New(world_createEntity));
	
	return hs.Close( world );
}

REG_SCRIPT_GLOBAL_TEMPLATE( world, &initWorld );
