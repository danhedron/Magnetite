#include "BlockFactory.h"

BaseBlockFactory::BaseBlockFactory( std::string type )
{ 
	typeName = type; 
	// Register this factory with the factory manager.
	FactoryManager::getManager().registerFactory( this );
}

std::string BaseBlockFactory::getType() 
{ 
	return typeName; 
}

BaseBlock* BaseBlockFactory::create() 
{ 
	Util::log("Error: Create called on BaseFactory :- use GenericFactory instead."); 
	return NULL; 
}

