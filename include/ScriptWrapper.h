#ifndef _SCRIPTWRAPPER_H_
#define _SCRIPTWRAPPER_H_
#include <v8.h>
#include "prerequisites.h"

typedef v8::Persistent<v8::Context> PersistentContext;
typedef v8::Persistent<v8::Object> PersistentObject;
typedef v8::Handle<v8::Value> ValueHandle;
// It's types all the way down
typedef std::map<std::string, v8::Persistent<v8::Object>> GameTypePrototypes;

class ScriptWrapper
{
	PersistentContext mContext;
	
public:
	PersistentContext getContext();
	
	/**
	 * Loads a game type's script into the cache of game types
	 */
	PersistentObject loadGame( const std::string& name );
	
	/**
	 * Creates a brand new instance of a game type, cloning the cache entry (and loading it if it doesn't exist)
	 */
	PersistentObject newGame( const std::string& name );
	
	void runFile( std::string filename );

	void init();
};


#endif
