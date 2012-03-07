#ifndef _SCRIPTGLOBALSLIST_H_
#define _SCRIPTGLOBALSLIST_H_
#include <v8.h>
#include "prerequisites.h"

typedef v8::Handle<v8::ObjectTemplate> (*ScriptGlobalFunction)();
typedef std::map<std::string, ScriptGlobalFunction> GlobalScriptTemplates ;

class ScriptGlobalsList 
{
protected:
	GlobalScriptTemplates templates;
	
public:
	
	void addGlobal( std::string globalName, ScriptGlobalFunction func )
	{
		templates[globalName] = func;
	}
	
	GlobalScriptTemplates& getTemplates()
	{
		return templates;
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

#endif
