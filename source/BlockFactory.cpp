#include "BlockFactory.h"

// =====================================================
// == Block Factories

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

// =====================================================
// == Game Type Factories

BaseGameFactory::BaseGameFactory( std::string type )
{ 
	typeName = type; 
	// Register this factory with the factory manager.
	FactoryManager::getManager().registerGame( this );
}

std::string BaseGameFactory::getType() 
{ 
	return typeName; 
}

BaseGame* BaseGameFactory::create() 
{ 
	Util::log("Error: Create called on BaseFactory :- use GenericFactory instead."); 
	return NULL; 
}