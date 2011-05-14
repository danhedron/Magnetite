#include "BlockFactory.h"

BlockFactoryList FactoryManager::blockFactoryList = BlockFactoryList();

BaseBlockFactory::BaseBlockFactory( std::string type )
{ 
	typeName = type; 
	// Register this factory with the factory manager.
	FactoryManager::registerFactory( this );
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

