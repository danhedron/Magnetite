#ifndef _SCRIPTGLOBALSLIST_H_
#define _SCRIPTGLOBALSLIST_H_
#include <v8.h>
#include "prerequisites.h"

/**
 * Typedefs for the Global Object Constructors
 */
typedef v8::Handle<v8::ObjectTemplate> (*ScriptGlobalFunction)();
typedef std::map<std::string, ScriptGlobalFunction> GlobalScriptTemplates;

/**
 * Typedefs for the Global Functions
 */
typedef v8::Handle<v8::FunctionTemplate> (*ScriptFunction)();
typedef std::map<std::string, ScriptFunction> GlobalScriptFunctions;

class ScriptGlobalsList 
{
protected:
	GlobalScriptTemplates templates;
	GlobalScriptFunctions functions;
public:
	
	void addGlobal( std::string globalName, ScriptGlobalFunction func )
	{
		templates[globalName] = func;
	}
	
	GlobalScriptTemplates& getTemplates()
	{
		return templates;
	}
	
	void addFunction( std::string functionName, ScriptFunction func )
	{
		functions[functionName] = func;
	}
	
	GlobalScriptFunctions& getFunctions()
	{
		return functions;
	}
	
	static ScriptGlobalsList& getList() 
	{
		static ScriptGlobalsList list;
		return list;
	}
	
};

#define REG_SCRIPT_GLOBAL_TEMPLATE( name, function ) \
	class ScriptTemplate##name { public: ScriptTemplate##name ( ) { ScriptGlobalsList::getList().addGlobal( std::string( #name ), function ); } }; \
	ScriptTemplate##name name##BoostrapObject;

#define REG_SCRIPT_GLOBAL_FUNCTION( name, function ) \
	class ScriptFunction##name { public: ScriptFunction##name ( ) { ScriptGlobalsList::getList().addFunction( std::string( #name ), function ); } }; \
	ScriptFunction##name name##BoostrapFunction;

#endif
