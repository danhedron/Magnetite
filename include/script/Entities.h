#ifndef _ENTITIES_H_
#define _ENTITIES_H_
#include <v8.h>
#include "prerequisites.h"

v8::Handle<v8::Value> wrapEntity( Magnetite::BaseEntity* ent );

#endif