#include "script/ScriptEntity.h"
#include <MagnetiteCore.h>

using namespace v8;

namespace Magnetite {
	namespace Script {
		
		ScriptEntity::ScriptEntity( World* world )
		: BaseEntity(world)
		{
			
		}
		
		ScriptEntity::~ScriptEntity()
		{
			
		}
		
		void ScriptEntity::setObject( PersistentObject object )
		{
			mScriptObject = object;
		}
		
		void ScriptEntity::create()
		{
			// Perhaps a better (thread safe) mechanism for handling callbacks could be devised.
			PersistentContext ctx = MagnetiteCore::Singleton->getScriptManager()->getContext();
			Context::Scope scope( ctx );
			HandleScope hs;
			// Get the object prototype, which contains the script-side create.
			auto proto = mScriptObject->GetPrototype();
			if( proto->IsObject() )
			{
				auto protobj = proto.As<Object>();
				if( protobj->Has( v8::String::New("create") ) ) {
					auto createVal = protobj->Get( v8::String::New("create") );
					if( createVal->IsFunction() )
					{
						auto createFunc = createVal.As<Function>();
						createFunc->Call( mScriptObject, 0, nullptr );
					}
				}
			}
			
			BaseEntity::create();
		}
		
		void ScriptEntity::think( float dt )
		{
			// Perhaps a better (thread safe) mechanism for handling callbacks could be devised.
			PersistentContext ctx = MagnetiteCore::Singleton->getScriptManager()->getContext();
			Context::Scope scope( ctx );
			HandleScope hs;
			// Get the object prototype, which contains the script-side create.
			auto proto = mScriptObject->GetPrototype();
			if( proto->IsObject() )
			{
				auto protobj = proto.As<Object>();
				if( protobj->Has( v8::String::New("think") ) ) {
					auto createVal = protobj->Get( v8::String::New("think") );
					if( createVal->IsFunction() )
					{
						auto createFunc = createVal.As<Function>();
						ValueHandle args[] = { Number::New(dt) };
						createFunc->Call( mScriptObject, 1, args );
					}
				}
			}
			BaseEntity::think(dt);
		}
		
	};
};
