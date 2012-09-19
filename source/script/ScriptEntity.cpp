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
			if( mScriptObject->Has( v8::String::New("create") ) )
			{
				auto createVal = mScriptObject->Get( v8::String::New("create") );
				if( createVal->IsFunction() )
				{
					auto createFunc = createVal.As<Function>();
					createFunc->Call( mScriptObject, 0, nullptr );
				}
			}
			
			BaseEntity::create();
		}
		
		void ScriptEntity::think( float dt )
		{
			/*PersistentContext ctx = MagnetiteCore::Singleton->getScriptManager()->getContext();
			Context::Scope scope( ctx );
			HandleScope hs;
			if( mScriptObject->Has( v8::String::New("think") ) )
			{
				auto createVal = mScriptObject->Get( v8::String::New("think") );
				if( createVal->IsFunction() )
				{
					auto createFunc = createVal.As<Function>();
					createFunc->Call( mScriptObject, 0 , nullptr);
				}
			}*/
			
			BaseEntity::think(dt);
		}
		
	};
};
