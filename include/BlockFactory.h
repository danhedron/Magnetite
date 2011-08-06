#ifndef _BLOCKFACTORY_H_
#define _BLOCKFACTORY_H_
#include "prerequisites.h"

class BaseBlockFactory {
protected:
	std::string typeName;
public:
	BaseBlockFactory( std::string type );
	std::string getType();
	virtual BaseBlock* create();
};

typedef std::map<std::string,BaseBlockFactory*> BlockFactoryList;

class FactoryManager {
public:
	/**
	 * Contains the list of factories that can create blocks.
	 */
	BlockFactoryList blockFactoryList;
	static FactoryManager& getManager()
	{
		static FactoryManager fmgr;
		return fmgr;
	}
	/**
	 * Registers a new factory 
	 * @param factory Factory Object.
	 */
	void registerFactory( BaseBlockFactory* factory )
	{
		blockFactoryList.insert( BlockFactoryList::value_type( factory->getType(), factory ) );
	}
	/**
	 * Logs available factories to the log
	 */
	void logFactories()
	{
		Util::log("Block Types:");
		for( BlockFactoryList::iterator it = blockFactoryList.begin(); it != blockFactoryList.end(); ++it )
			Util::log( "\t" + (*it).first );
	}
	/**
	 * Creates a factory of the specifed type
	 */
	BaseBlock* createBlock( std::string type ) {
		BlockFactoryList::iterator it = blockFactoryList.find( type );
		if( it != blockFactoryList.end() ) {
			return (*it).second->create();
		}
		return NULL;
	}
};

template<class T> class GenericBlockFactory : BaseBlockFactory {
public:
	GenericBlockFactory( std::string type ) : BaseBlockFactory( type ) { }
	virtual BaseBlock* create() { return new T; }
};

// Obligatory registration macro
//#define REGISTER_BLOCK_TYPE( name, type ) \
//	GenericBlockFactory<type> BlockFactory( type );

#endif // _BLOCKFACTORY_H_