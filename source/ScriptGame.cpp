#include "ScriptGame.h"
#include "BlockFactory.h"
#include "Character.h"
#include "MagnetiteCore.h"
#include "Renderer.h"
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
		mPlayer = createCharacter();
		playerJoin( mPlayer );
		playerSpawn( mPlayer );
	}
}

Character* ScriptGame::getLocalPlayer()
{
	return mPlayer;
}

Character* ScriptGame::createCharacter()
{
	return mEngine->createCharacter();
}

void ScriptGame::_inputEvents( const InputEvent& e )
{
	if( e.event == Inputs::FORWARD ) {
		if( e.down )
			_inputMovement( Vector3( 0.f, 0.f, -1.f ) );
		else
			_inputMovement( Vector3( 0.f, 0.f, 1.f ) );
	}
	if( e.event == Inputs::LEFT ) {
		if( e.down )
			_inputMovement( Vector3( -1.f, 0.f, 0.f ) );
		else
			_inputMovement( Vector3( 1.f, 0.f, 0.f ) );
	}
	if( e.event == Inputs::RIGHT ) {
		if( e.down )
			_inputMovement( Vector3( 1.f, 0.f, 0.f ) );
		else
			_inputMovement( Vector3( -1.f, 0.f, 0.f ) );
	}
	if( e.event == Inputs::BACK ) {
		if( e.down )
			_inputMovement( Vector3( 0.f, 0.f, 1.f ) );
		else
			_inputMovement( Vector3( 0.f, 0.f, -1.f ) );
	}
	if( e.event == Inputs::JUMP && e.down ) {
		if( getLocalPlayer() ) mPlayer->jump();
	}
	if( e.event == Inputs::SPRINT ) {
		if( getLocalPlayer() ) mPlayer->enableSprint( e.down );
	}
	if( e.event == Inputs::FLY && e.down ) {
		if( getLocalPlayer() ) mPlayer->enableFlying( !mPlayer->isFlying() );
	}
	if( e.event == Inputs::RESPAWN && e.down ) {
		if( getLocalPlayer() ) mPlayer->setPosition( Vector3( 0, 150.f, 0 ) );
	}
}

void ScriptGame::_inputMovement( const Vector3& v )
{
	if( getLocalPlayer() )
		mPlayer->addMoveDelta( v );
}

void ScriptGame::_mouseMoved( const float x, const float y )
{
	if( getLocalPlayer() ) {
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

void ScriptGame::playerJoin( Character* player )
{
	HandleScope hs;
	if( !mScriptObject.IsEmpty() && mScriptObject->Has( String::New("onJoin") ) )
	{
		Local<Value> onLoadVal = mScriptObject->Get( String::New("onJoin") );
		if( onLoadVal->IsFunction() )
		{
			Local<Function> onLoad = Local<Function>::Cast( onLoadVal );
			onLoad->Call( mScriptObject, 0, NULL );
		}
	}
}

void ScriptGame::playerSpawn( Character* player )
{
	HandleScope hs;
	if( !mScriptObject.IsEmpty() && mScriptObject->Has( String::New("onSpawn") ) )
	{
		Local<Value> onLoadVal = mScriptObject->Get( String::New("onSpawn") );
		if( onLoadVal->IsFunction() )
		{
			Local<Function> onLoad = Local<Function>::Cast( onLoadVal );
			onLoad->Call( mScriptObject, 0, NULL );
		}
	}

	player->setPosition( Vector3( 0.f, 120.f, 0.f )  );
}

void ScriptGame::playerKilled( Character* player )
{
	if( player == mPlayer )
		Util::log( "You just died! D:" );
}

void ScriptGame::characterDamage( Character* player )
{
	if( player == mPlayer )
		Util::log( "You're taking damage" );
}

void ScriptGame::playerPrimaryClick( Character* player )
{
	raycast_r ray = player->getEyeCast();
	ray = mEngine->getWorld()->raycastWorld(ray);
	ray.maxDistance = 10;
	if(ray.hit)
	{	
		if( clickMode == "remove" )
		{
			if(ray.chunk && ray.block) 
			{
				ray.chunk->removeBlockAt( ray.blockIndex );
			}
		}
		else
		{
			explosion_t info;
			info.center = ray.worldHit + ray.hitNormal * 1.5;
			Explosion expl(info);
			expl.explode();
		}
	}
}

void ScriptGame::playerAltClick( Character* player )
{
	raycast_r ray = player->getEyeCast();
	ray = mEngine->getWorld()->raycastWorld(ray);
	if(ray.hit && ray.block)
	{
		BaseBlock* block = FactoryManager::getManager().createBlock( mEngine->getRenderer()->blockType );
		if( block != NULL ) {
			mEngine->getWorld()->setBlockAt( block, ray.worldHit.x + ray.hitNormal.x/2, ray.worldHit.y + ray.hitNormal.y/2, ray.worldHit.z + ray.hitNormal.z/2 );
		}
	}
}

void ScriptGame::uiPaint(Renderer* r)
{
	r->drawText(clickMode, 10, 50);
	
	HandleScope hs;
	PersistentContext ctx = MagnetiteCore::Singleton->getScriptManager()->getContext();
	Context::Scope scope( ctx );
	
	if( !mScriptObject.IsEmpty() && mScriptObject->Has( String::New("draw") ) )
	{
		Local<Value> onLoadVal = mScriptObject->Get( String::New("draw") );
		if( onLoadVal->IsFunction() )
		{
			Local<Function> onLoad = Local<Function>::Cast( onLoadVal );
			onLoad->Call( mScriptObject, 0, NULL );
		}
	}
}

void ScriptGame::keyDown( size_t evt )
{
	if( evt == sf::Keyboard::M )
	{
		if( clickMode == "remove" )
			clickMode = "explode";
		else clickMode = "remove";
	}
}

void ScriptGame::keyUp( size_t evt )
{
}