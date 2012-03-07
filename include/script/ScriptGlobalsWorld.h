#ifndef _SCRIPTGLOBALSWORLD_H_
#define _SCRIPTGLOBALSWORLD_H_
#include "ScriptWrapper.h"
#include "World.h"
#include "prerequisites.h"

/**
 * These (evil) global functions are used elsewhere
 */
ValueHandle constructRay( const v8::Arguments& args );
bool unwrapRay( v8::Local<v8::Value> ray, raycast_r& r );
ValueHandle wrapRay( const raycast_r& ray );

#endif
