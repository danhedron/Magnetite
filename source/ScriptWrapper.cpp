#include "ScriptWrapper.h"
#include "MagnetiteCore.h"
#include "World.h"
#include "BaseBlock.h"
#include "BlockFactory.h"
#include <fstream>
using namespace v8;

std::string strize( Handle<Value> s )
{
	String::Utf8Value v(s);
	return *v ? *v : "";
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

ValueHandle runFile( std::string filename )
{
	std::ifstream is(filename.c_str(), std::ios_base::in);
	
	if(is.is_open())
	{
		std::string source;
		std::string line;
		
		while( !is.eof() )
		{
			std::getline(is, line);
			source.append(line);
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
		
		ValueHandle result = script->Run();
		if(try_catch.HasCaught())
		{
			report(&try_catch);
			return Undefined();
		}
		return scope.Close(result);
	}
	else
	{
		Util::log("Unable to open: " + filename);
	}
	
	return Undefined();
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
		return runFile( strize( args[0]->ToString() ) );
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
	
	Handle<ObjectTemplate> world = ObjectTemplate::New();
	world->Set(String::New("getBlock"), FunctionTemplate::New(world_getBlock));
	world->Set(String::New("removeBlock"), FunctionTemplate::New(world_removeBlock));
	world->Set(String::New("createBlock"), FunctionTemplate::New(world_createBlock));
	
	global->Set(String::New("console"), console);
	global->Set(String::New("world"), world);
	
	mContext = Context::New(NULL, global);
	
	Context::Scope ctx_scope(mContext);
	
	runFile("./scripts/main.js");
}
