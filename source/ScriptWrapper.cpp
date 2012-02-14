#include "ScriptWrapper.h"
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
			veco->Get( String::New("x") )->Int32Value(),
			veco->Get( String::New("y") )->Int32Value(),
			veco->Get( String::New("z") )->Int32Value()
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
	}
	
	PersistentObject pl = PersistentObject::New(playerTemplate->NewInstance());
	gWrappedPlayers.insert( WrappedPlayers::value_type( player, pl ) );
	
	pl->SetInternalField(0, External::New(player));
	
	return hs.Close(pl);
}

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
		blockTemplate->Set( String::New( "type" ), String::New( block->getType().c_str() ) );
	}
	
	PersistentObject obj = Persistent<Object>::New(blockTemplate->NewInstance());
	gWrappedBlocks.insert( WrappedBlocks::value_type( block, obj) );
	
	return obj;
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
		
		ValueHandle result = script->Run();
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

ValueHandle log(const Arguments& args)
{
	for( size_t i = 0; i < args.Length(); i++ )
	{
		Util::log( strize(args[i]->ToString() ));
	}
}

ValueHandle import(const Arguments& args)
{
	if( args.Length() >= 1 )
	{
		MagnetiteCore::Singleton->getScriptManager()->runFile( strize( args[0]->ToString() ) );
	}
	return Undefined();
}

ValueHandle debugText(const Arguments& args)
{
	if( args.Length() >= 3 )
	{
		int x = args[0]->Int32Value();
		int y = args[1]->Int32Value();
		std::string text = strize( args[2] );
		
		MagnetiteCore::Singleton->getRenderer()->drawText( text, x, y );
	}
	return Undefined();
}

/*=========== World Object functions ===========*/
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

void ScriptWrapper::init()
{
	HandleScope scope;
	
	Handle<ObjectTemplate> global = ObjectTemplate::New();
	global->Set(String::New("import"), FunctionTemplate::New(import));
	
	Handle<ObjectTemplate> console = ObjectTemplate::New();
	console->Set(String::New("log"), FunctionTemplate::New(log));
	console->Set(String::New("drawText"), FunctionTemplate::New(debugText));
	
	Handle<ObjectTemplate> world = ObjectTemplate::New();
	world->Set(String::New("getBlock"), FunctionTemplate::New(world_getBlock));
	world->Set(String::New("removeBlock"), FunctionTemplate::New(world_removeBlock));
	world->Set(String::New("createBlock"), FunctionTemplate::New(world_createBlock));
	
	global->Set(String::New("console"), console);
	global->Set(String::New("debug"), console);
	global->Set(String::New("world"), world);
	
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