#ifndef _SCRIPTWRAPPER_H_
#define _SCRIPTWRAPPER_H_
#include <v8.h>
#include "prerequisites.h"

typedef v8::Persistent<v8::Context> PersistentContext;
typedef v8::Handle<v8::Value> ValueHandle;

class ScriptWrapper
{
	PersistentContext mContext;
	
public:
	PersistentContext getContext();

	void init();
};


#endif
