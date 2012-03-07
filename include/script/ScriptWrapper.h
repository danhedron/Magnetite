#ifndef _SCRIPTWRAPPER_H_
#define _SCRIPTWRAPPER_H_
#include <v8.h>
#include "prerequisites.h"

typedef v8::Persistent<v8::Context> PersistentContext;
typedef v8::Persistent<v8::Object> PersistentObject;
typedef v8::Handle<v8::Value> ValueHandle;
typedef v8::Handle<v8::Object> ObjectHandle;
// It's types all the way down
typedef std::map<std::string, v8::Persistent<v8::Object>> GameTypePrototypes;

/**
 * Evil global functions
 */
class Player;
ValueHandle wrapPlayer( Player* player );

std::string strize( v8::Handle<v8::Value> s );

ValueHandle wrapVector3( const Vector3& v );

Vector3 unwrapVector3( ValueHandle v );

class ScriptWrapper
{
	PersistentContext mContext;
	
public:
	
	/**
	 * Loads a game type's script into the cache of game types
	 */
	PersistentObject loadGame( const std::string& name );
	
	/**
	 * Creates a brand new instance of a game type, cloning the cache entry (and loading it if it doesn't exist)
	 */
	PersistentObject newGame( const std::string& name );
	
	void runFile( std::string filename );
	
	PersistentContext getContext();

	void init();
};


#endif
