#include "BaseGame.h"
#include "BlockFactory.h"
#include "Character.h"
#include "Player.h"
#include "MagnetiteCore.h"
#include "Renderer.h"
#include "Chunk.h"
#include "BaseBlock.h"
#include "Camera.h"
#include "World.h"

REG_GAME_TYPE( "default", BaseGame )

BaseGame::BaseGame()
: mEngine(MagnetiteCore::Singleton),
mPlayer(NULL)
{
}

BaseGame::~BaseGame()
{

}

std::string BaseGame::getName()
{
	return "Base";
}

Magnetite::String BaseGame::getPath()
{
	return "";
}

void BaseGame::_startGameSingle()
{
	mIsSinglePlayer = true;
	_startGame();
}

void BaseGame::_startGame()
{
	Util::log("#== Base Game Type ==================");
	Util::log("# is multiplayer: " + Util::toString( !isSingleplayer() ) );
}

void BaseGame::_loadGame()
{
	
}

bool BaseGame::isSingleplayer()
{
	return mIsSinglePlayer;
}

void BaseGame::_playerJoined()
{
	if( isSingleplayer() ) {
		mPlayer = new Player();
		mEngine->addPlayer(mPlayer);
		playerJoin( mPlayer );
		playerSpawn( mPlayer );
	}
}

Player* BaseGame::getLocalPlayer()
{
	return mPlayer;
}

void BaseGame::_inputEvents( const InputEvent& e )
{

}

void BaseGame::_inputMovement( const Vector3& v )
{
}

void BaseGame::_mouseMoved( const float x, const float y )
{
	if( getLocalPlayer() ) {
		mPlayer->getCamera()->pitch( y );
		mPlayer->getCamera()->yaw( x );
	}
}

void BaseGame::_primary()
{
	if( getLocalPlayer() ) {
		playerPrimaryClick( mPlayer );
	}
}

void BaseGame::_secondary()
{
	if( getLocalPlayer() ) {
		playerAltClick( mPlayer );
	}
}

//========= Events

void BaseGame::playerJoin( Player* player )
{
	Util::log( "A player just joined!" );
}

void BaseGame::playerSpawn( Player* player )
{
	player->setPosition( Vector3( 0.f, 120.f, 0.f )  );
}

void BaseGame::playerKilled( Character* player )
{
}

void BaseGame::characterDamage( Character* player )
{
}

void BaseGame::playerPrimaryClick( Player* player )
{
	
}

void BaseGame::playerAltClick( Player* player )
{
	
}

void BaseGame::uiPaint(Renderer* r)
{
}

void BaseGame::think( float dt )
{
	
}
void BaseGame::keyDown( size_t evt )
{
	
}

void BaseGame::keyUp( size_t evt )
{
	
}