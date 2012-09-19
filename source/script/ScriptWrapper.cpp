#include "ScriptWrapper.h"
#include "ScriptGlobalsList.h"
#include "ScriptGlobalsWorld.h"
#include "MagnetiteCore.h"
#include "Renderer.h"
#include "World.h"
#include "Player.h"
#include "BaseBlock.h"
#include "BlockFactory.h"
#include <BaseGame.h>
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
	
	
	Util::log( strize(handler->StackTrace()->ToString()) );

	//return;
	
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

ValueHandle ScriptWrapper::runFile( std::string filename )
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
				return Undefined();
			}
		}
		
		auto r = script->Run();
		
		if(r.IsEmpty())
		{
			report(&try_catch);
			return Undefined();
		}
		return r;
	}
	else
	{
		Util::log("Unable to open: " + filename);
	}
	
	return Undefined();
}

ValueHandle import(const Arguments& args)
{
	if( args.Length() >= 1 )
	{
		Util::log( "Importing: " + strize( args[0]->ToString() ) );
		return MagnetiteCore::Singleton->getScriptManager()->runFile( strize( args[0]->ToString() ) );
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
	global->Set(String::New("require"), FunctionTemplate::New(import));

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
	
	GlobalScriptFunctions functions = ScriptGlobalsList::getList().getFunctions();
	for( GlobalScriptFunctions::iterator it = functions.begin(); it != functions.end(); ++it )
	{
		Handle<FunctionTemplate> f = (it->second)();
		global->Set(String::New( it->first.c_str() ), f);
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

Magnetite::String ScriptWrapper::resolveEntityPath( const Magnetite::String& name )
{
	// Check in order if any of the valid directories exist; todo: make this less so.
	auto currGame = MagnetiteCore::Singleton->getGame();
	
	std::ifstream test;
	if( currGame ) {
		auto p = std::string("./scripts/games/" + currGame->getPath() + "/entities/" + name + "/" + name + ".js");
		test.open(p.c_str());
		if( test.is_open() ) return p;
	}
	
	auto p = std::string("./scripts/entities/" + name + "/" + name + ".js");
	test.open(p.c_str());
	if( test.is_open() ) return p;
	
	// This entity isn't even valid.
	return "";
}