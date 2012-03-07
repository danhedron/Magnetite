#include "ScriptWrapper.h"
#include "ScriptGlobalsList.h"
#include "ScriptGlobalsWorld.h"
#include "MagnetiteCore.h"
#include "Renderer.h"
#include "World.h"
#include "Player.h"
#include "BaseBlock.h"
#include "BlockFactory.h"
#include <fstream>


using namespace v8;

std::string strize( Handle<Value> s )
{
	String::Utf8Value v(s);
	return *v ? *v : "";
}

ValueHandle wrapVector3( const Vector3& v )
{
	HandleScope hs;
	
	Local<Object> vec = Object::New();
	vec->Set( String::New("x"), Number::New( v.x ) );
	vec->Set( String::New("y"), Number::New( v.y ) );
	vec->Set( String::New("z"), Number::New( v.z ) );
	
	return hs.Close( vec );
}

Vector3 unwrapVector3( ValueHandle v )
{
	HandleScope hs;
	if( !v->IsObject() ) return Vector3();
	
	ObjectHandle veco = ObjectHandle::Cast( v );
	
	return Vector3( 
			veco->Get( String::New("x") )->NumberValue(),
			veco->Get( String::New("y") )->NumberValue(),
			veco->Get( String::New("z") )->NumberValue()
		 );
}

void report( TryCatch* handler )
{
	HandleScope scope;
	
	if(handler->Message().IsEmpty())
	{
		// No message
		Util::log( strize( handler->Exception() ) );
	}
	else
	{
		Handle<Message> message = handler->Message();
		std::string file = strize( message->GetScriptResourceName() );
		int lnnum = message->GetLineNumber();
		Util::log( file + ":" + Util::toString(lnnum) + " " + strize( handler->Exception() ) );
	}
}

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
	}
	
	PersistentObject pl = PersistentObject::New(playerTemplate->NewInstance());
	gWrappedPlayers.insert( WrappedPlayers::value_type( player, pl ) );
	
	pl->SetInternalField(0, External::New(player));
	
	return hs.Close(pl);
}

void ScriptWrapper::runFile( std::string filename )
{
	std::ifstream is(filename.c_str(), std::ios_base::in);
	Context::Scope ctx_scope(mContext);
	
	if(is.is_open())
	{
		std::string source;
		std::string line;
		
		while( !is.eof() )
		{
			std::getline(is, line);
			source.append(line + "\n");
		}
		
		HandleScope scope;
		TryCatch try_catch;
		
		Handle<Script> script = Script::Compile( String::New(source.c_str()), String::New(filename.c_str()) );
		if( script.IsEmpty() )
		{
			if(try_catch.HasCaught())
			{
				report(&try_catch);
				return;
			}
		}
		
		script->Run();
		
		if(try_catch.HasCaught())
		{
			report(&try_catch);
			return;
		}
		return;
	}
	else
	{
		Util::log("Unable to open: " + filename);
	}
	
	return;
}

ValueHandle import(const Arguments& args)
{
	if( args.Length() >= 1 )
	{
		MagnetiteCore::Singleton->getScriptManager()->runFile( strize( args[0]->ToString() ) );
	}
	return Undefined();
}

/**
 * Meta api
 */
ValueHandle meta_blocks_availableTypes(Local<String> prop,
						   const AccessorInfo& info) {
	HandleScope hs;
	
	BlockFactoryList& list = FactoryManager::getManager().blockFactoryList;
	Local<Array> types = Array::New(list.size());
	size_t i = 0;
	for( BlockFactoryList::iterator it = list.begin(); it != list.end(); ++it )
	{
		types->Set( Number::New( i++ ), String::New((*it).first.c_str()) );
	}
	
	return hs.Close( types );
}

void ScriptWrapper::init()
{
	HandleScope scope;
	
	Handle<ObjectTemplate> global = ObjectTemplate::New();
	global->Set(String::New("import"), FunctionTemplate::New(import));

	Handle<ObjectTemplate> meta = ObjectTemplate::New();
	Handle<ObjectTemplate> blocks = ObjectTemplate::New();
	blocks->SetAccessor(String::New("availableTypes"), meta_blocks_availableTypes, NULL,
				  Handle<Value>(),
				  PROHIBITS_OVERWRITING,
			   ReadOnly);
	meta->Set(String::New("blocks"), blocks);
	
	// Create the globals
	GlobalScriptTemplates templates = ScriptGlobalsList::getList().getTemplates();
	for( GlobalScriptTemplates::iterator it = templates.begin(); it != templates.end(); ++it )
	{
		// Registers the return value of the global's function 
		// in the global variable name given
		ObjectTemplateHandle glob = (it->second)();
		global->Set(String::New( it->first.c_str() ), glob);
		// I thought the original system wasn't hacky enough so I replaced it with a more complex series of hacks.
		if( it->first == "debug" ) 
		{
			global->Set(String::New("console"), glob);
		}
	}
	
	global->Set(String::New("meta"), meta);
	global->Set(String::New("Ray"), FunctionTemplate::New(constructRay));
	
	mContext = Context::New(NULL, global);
	
	//Context::Scope ctx_scope(mContext);
}

PersistentContext ScriptWrapper::getContext()
{
	return mContext;
}

PersistentObject ScriptWrapper::loadGame( const std::string& name )
{
	HandleScope hs;
	
	// This is polluting the global context, loading could be done in a seperate context.
	Context::Scope scope( mContext );
	
	mContext->Global()->Set( String::New("Game"), Object::New() );
	
	runFile("./scripts/games/" + name + "/"+ name + ".js");
	
	Local<Value> gameVal =  mContext->Global()->Get( String::New("Game"));
	
	Local<Object> gameObj = Local<Object>::Cast(gameVal);
	
	return Persistent<Object>::New(gameObj);
}

PersistentObject ScriptWrapper::newGame( const std::string& name )
{
	HandleScope hs;
	
	// This is polluting the global context, loading could be done in a seperate context.
	Context::Scope scope( mContext );
	
	// There is no cache right now
	PersistentObject game = loadGame(name);
		
	return game;
}