#include "ScriptGame.h"
#include "BlockFactory.h"
#include "Character.h"
#include "MagnetiteCore.h"
#include "Renderer.h"
#include "Player.h"
#include "Chunk.h"
#include "BaseBlock.h"
#include "Camera.h"
#include "World.h"
#include "Explosion.h"

//Handled by magic 
REG_GAME_TYPE( "script", ScriptGame )

using namespace v8;

ScriptGame::ScriptGame()
{
	mEngine = MagnetiteCore::Singleton; // Bad design they say? Humbug!
	mPlayer = NULL;
	clickMode = "remove";
}

ScriptGame::~ScriptGame()
{

}

std::string ScriptGame::getName()
{
	HandleScope hs;
	if( !mScriptObject.IsEmpty() )
	{
		if( mScriptObject->Has( String::New("name") ) )
		{
			Local<Value> nameVal = mScriptObject->Get( String::New("name") );
			String::Utf8Value nameStr( nameVal );
			return *nameStr;
		}
	}
	return "Un-named Game Type";
}

void ScriptGame::setName( const std::string& name )
{
	mGameName = name;
}

void ScriptGame::_setScriptObject( PersistentObject object )
{
	mScriptObject = object;
}

void ScriptGame::_startGameSingle()
{
	mIsSinglePlayer = true;
	_startGame();
}

void ScriptGame::_startGame()
{
	HandleScope hs;
	PersistentContext ctx = MagnetiteCore::Singleton->getScriptManager()->getContext();
	Context::Scope scope( ctx );
	Util::log("#== " + getName() + " ==================");
	if( !mScriptObject.IsEmpty() && mScriptObject->Has( String::New("onStart") ) )
	{
		Local<Value> onStartVal = mScriptObject->Get( String::New("onStart") );
		if( onStartVal->IsFunction() )
		{
			Local<Function> onStart = Local<Function>::Cast( onStartVal );
			onStart->Call( mScriptObject, 0, NULL );
		}
	}
}

void ScriptGame::_loadGame()
{
	HandleScope hs;
	PersistentContext ctx = MagnetiteCore::Singleton->getScriptManager()->getContext();
	Context::Scope scope( ctx );
	if( !mScriptObject.IsEmpty() && mScriptObject->Has( String::New("onLoad") ) )
	{
		Local<Value> onLoadVal = mScriptObject->Get( String::New("onLoad") );
		if( onLoadVal->IsFunction() )
		{
			Local<Function> onLoad = Local<Function>::Cast( onLoadVal );
			onLoad->Call( mScriptObject, 0, NULL );
		}
	}
}

bool ScriptGame::isSingleplayer()
{
	return mIsSinglePlayer;
}

void ScriptGame::_playerJoined()
{
	if( isSingleplayer() ) {
		mPlayer = new Player();
		playerJoin( mPlayer );
		playerSpawn( mPlayer );
	}
}

void ScriptGame::_inputEvents( const InputEvent& e )
{
}

void ScriptGame::_inputMovement( const Vector3& v )
{
	
}

void ScriptGame::_mouseMoved( const float x, const float y )
{
	HandleScope hs;
	PersistentContext ctx = MagnetiteCore::Singleton->getScriptManager()->getContext();
	Context::Scope scope( ctx );
	
	bool eval = false;
	if( !mScriptObject.IsEmpty() && mScriptObject->Has( String::New("mouseMoved") ) )
	{
		Local<Value> onLoadVal = mScriptObject->Get( String::New("mouseMoved") );
		if( onLoadVal->IsFunction() )
		{
			TryCatch ct;
			Local<Function> onLoad = Local<Function>::Cast( onLoadVal );
			Handle<Value> args[2];
			args[0] = Number::New(x);
			args[1] = Number::New(y);
			auto r = onLoad->Call( mScriptObject, 2, args );
			if( r.IsEmpty() ) {
				Util::log(strize(ct.StackTrace()));
			}
			else 
			{
				eval = r->BooleanValue();
			}
		}
	}
	if(!eval && getLocalPlayer() ) {
		mPlayer->getCamera()->pitch( y );
		mPlayer->getCamera()->yaw( x );
	}
}

void ScriptGame::_primary()
{
	if( getLocalPlayer() ) {
		playerPrimaryClick( mPlayer );
	}
}

void ScriptGame::_secondary()
{
	if( getLocalPlayer() ) {
		playerAltClick( mPlayer );
	}
}

//========= Events

void ScriptGame::playerJoin( Player* player )
{
	HandleScope hs;
	PersistentContext ctx = MagnetiteCore::Singleton->getScriptManager()->getContext();
	Context::Scope scope( ctx );
	if( !mScriptObject.IsEmpty() && mScriptObject->Has( String::New("onJoin") ) )
	{
		Local<Value> onLoadVal = mScriptObject->Get( String::New("onJoin") );
		if( onLoadVal->IsFunction() )
		{
			Local<Function> onLoad = Local<Function>::Cast( onLoadVal );
			Handle<Value> args[1];
			args[0] = wrapPlayer( player );
			onLoad->Call( mScriptObject, 1, args );
		}
	}
}

void ScriptGame::playerSpawn( Player* player )
{
	HandleScope hs;
	PersistentContext ctx = MagnetiteCore::Singleton->getScriptManager()->getContext();
	Context::Scope scope( ctx );
	if( !mScriptObject.IsEmpty() && mScriptObject->Has( String::New("onSpawn") ) )
	{
		Local<Value> onLoadVal = mScriptObject->Get( String::New("onSpawn") );
		if( onLoadVal->IsFunction() )
		{
			Local<Function> onLoad = Local<Function>::Cast( onLoadVal );
			Handle<Value> args[1];
			args[0] = wrapPlayer( player );
			onLoad->Call( mScriptObject, 1, args );
		}
	}
}

void ScriptGame::playerKilled( Character* player )
{
	
}

void ScriptGame::characterDamage( Character* player )
{
	
}

void ScriptGame::playerPrimaryClick( Player* player )
{
	HandleScope hs;
	PersistentContext ctx = MagnetiteCore::Singleton->getScriptManager()->getContext();
	Context::Scope scope( ctx );
	if( !mScriptObject.IsEmpty() && mScriptObject->Has( String::New("onPrimary") ) )
	{
		Local<Value> onLoadVal = mScriptObject->Get( String::New("onPrimary") );
		if( onLoadVal->IsFunction() )
		{
			Local<Function> onLoad = Local<Function>::Cast( onLoadVal );
			Handle<Value> args[1];
			args[0] = wrapPlayer( player );
			onLoad->Call( mScriptObject, 1, args );
		}
	}
}

void ScriptGame::playerAltClick( Player* player )
{
	HandleScope hs;
	PersistentContext ctx = MagnetiteCore::Singleton->getScriptManager()->getContext();
	Context::Scope scope( ctx );
	if( !mScriptObject.IsEmpty() && mScriptObject->Has( String::New("onAlt") ) )
	{
		Local<Value> onLoadVal = mScriptObject->Get( String::New("onAlt") );
		if( onLoadVal->IsFunction() )
		{
			Local<Function> onLoad = Local<Function>::Cast( onLoadVal );
			Handle<Value> args[1];
			args[0] = wrapPlayer( player );
			onLoad->Call( mScriptObject, 1, args );
		}
	}
}

void ScriptGame::uiPaint(Renderer* r)
{
	HandleScope hs;
	PersistentContext ctx = MagnetiteCore::Singleton->getScriptManager()->getContext();
	Context::Scope scope( ctx );
	
	if( !mScriptObject.IsEmpty() && mScriptObject->Has( String::New("draw") ) )
	{
		Local<Value> onLoadVal = mScriptObject->Get( String::New("draw") );
		if( onLoadVal->IsFunction() )
		{
			TryCatch ct;
			Local<Function> onLoad = Local<Function>::Cast( onLoadVal );
			auto r = onLoad->Call( mScriptObject, 0, NULL );
			if( r.IsEmpty() ) {
				Util::log(strize(ct.StackTrace()));
			}
		}
	}
}

void ScriptGame::think( float dt )
{	
	HandleScope hs;
	PersistentContext ctx = MagnetiteCore::Singleton->getScriptManager()->getContext();
	Context::Scope scope( ctx );
	
	if( !mScriptObject.IsEmpty() && mScriptObject->Has( String::New("think") ) )
	{
		Local<Value> onLoadVal = mScriptObject->Get( String::New("think") );
		if( onLoadVal->IsFunction() )
		{
			TryCatch ct;
			Local<Function> onLoad = Local<Function>::Cast( onLoadVal );
			Handle<Value> args[1];
			args[0] = Number::New( dt );
			auto r = onLoad->Call( mScriptObject, 1, args );
			if( r.IsEmpty() ) {
				Util::log(strize(ct.StackTrace()));
			}
		}
	}	
}

void ScriptGame::keyDown( size_t evt )
{
	HandleScope hs;
	PersistentContext ctx = MagnetiteCore::Singleton->getScriptManager()->getContext();
	Context::Scope scope( ctx );
	if( !mScriptObject.IsEmpty() && mScriptObject->Has( String::New("keyDown") ) )
	{
		Local<Value> onLoadVal = mScriptObject->Get( String::New("keyDown") );
		if( onLoadVal->IsFunction() )
		{
			Local<Function> onLoad = Local<Function>::Cast( onLoadVal );
			Handle<Value> args[1];
			args[0] = Number::New( evt );
			onLoad->Call( mScriptObject, 1, args );
		}
	}
}

void ScriptGame::keyUp( size_t evt )
{
	HandleScope hs;
	PersistentContext ctx = MagnetiteCore::Singleton->getScriptManager()->getContext();
	Context::Scope scope( ctx );
	if( !mScriptObject.IsEmpty() && mScriptObject->Has( String::New("keyUp") ) )
	{
		Local<Value> onLoadVal = mScriptObject->Get( String::New("keyUp") );
		if( onLoadVal->IsFunction() )
		{
			Local<Function> onLoad = Local<Function>::Cast( onLoadVal );
			Handle<Value> args[1];
			args[0] = Number::New( evt );
			onLoad->Call( mScriptObject, 1, args );
		}
	}
}