#include "ScriptGlobalsDebug.h"
#include "ScriptGlobalsList.h"
#include "ScriptWrapper.h"
#include "MagnetiteCore.h"
#include "Renderer.h"

using namespace v8;

ValueHandle debug_drawText(const Arguments& args)
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

ValueHandle debug_log(const Arguments& args)
{
	for( int i = 0; i < args.Length(); i++ )
	{
		Util::log( strize(args[i]->ToString() ));
	}
	return Undefined();
}

ObjectTemplateHandle constructDebugGlobal()
{
	HandleScope hs;
	
	Handle<ObjectTemplate> console = ObjectTemplate::New();
	console->Set(String::New("log"), FunctionTemplate::New(debug_log));
	console->Set(String::New("drawText"), FunctionTemplate::New(debug_drawText));
	
	return hs.Close(console);
}

REG_SCRIPT_GLOBAL_TEMPLATE( debug, &constructDebugGlobal );
