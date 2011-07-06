#include "InputManager.h"

InputManager::InputManager()
{
	/* Setup some default input events, this is a good a place as any. */
	createInput( sf::Key::W, Inputs::FORWARD );
	createInput( sf::Key::S, Inputs::BACK );
	createInput( sf::Key::A, Inputs::LEFT );
	createInput( sf::Key::D, Inputs::RIGHT );
	createInput( sf::Key::Space, Inputs::JUMP );
	createInput( sf::Key::LShift, Inputs::SPRINT );
}

InputManager::~InputManager()
{

}

std::string InputManager::inputToString( Inputs::Event evt )
{
	switch( evt ) {
		case Inputs::FORWARD:
			return "forward";
		case Inputs::BACK:
			return "back";
		case Inputs::LEFT:
			return "left";
		case Inputs::RIGHT:
			return "right";
		case Inputs::JUMP:
			return "jump";
		case Inputs::SPRINT:
			return "sprint";
	}
	return "";
}

void InputManager::createInput( sf::Key::Code key, Inputs::Event event )
{
	InputBinding input;
	input.event = event;
	input.key = key;
	input.callback = NULL;
	mInputs.push_back( input );
}

void InputManager::setEventCallback( Inputs::Event event, InputCallback callback )
{
	for(InputList::iterator it = mInputs.begin(); it != mInputs.end(); it++)
	{
		if( event == it->event )
			it->callback = callback;
	}
}

void InputManager::keyDown( sf::Key::Code key )
{
	for(InputList::iterator it = mInputs.begin(); it != mInputs.end(); it++)
	{
		if( key == it->key ) {
			InputEvent event;
			event.down = true;
			event.event = it->event;
			if( it->callback != NULL )
				it->callback( event );
		}
	}
}

void InputManager::keyUp( sf::Key::Code key )
{
	for(InputList::iterator it = mInputs.begin(); it != mInputs.end(); it++)
	{
		if( key == it->key ) {
			InputEvent event;
			event.down = false;
			event.event = it->event;
			if( it->callback != NULL )
				it->callback( event );
		}
	}
}

void InputManager::beginInput( Inputs::Event evt )
{
	
	Util::log("Received Input: " + inputToString( evt ));
}

void InputManager::endInput( Inputs::Event evt )
{
	Util::log("Finished Input: " + inputToString( evt ));
}
