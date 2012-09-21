#include "WorldSerializer.h"
#include <World.h>
#include <BaseBlock.h>
#include <BlockFactory.h>

namespace Magnetite
{
	WorldSerializer::WorldSerializer( World* w )
	: mWorld( w )
	{
		
	}
	
	WorldSerializer::~WorldSerializer()
	{
		
	}
	
	String WorldSerializer::resolveRegion( ChunkScalar x, ChunkScalar y, ChunkScalar z )
	{
		return "./worlds/" + mWorld->getName() + "/chunk" + Util::toString(x) + "." + Util::toString(y) + "." + Util::toString(z);
	}
	
	bool WorldSerializer::hasChunk( ChunkScalar x, ChunkScalar y, ChunkScalar z )
	{
		std::ifstream stream( resolveRegion( x, y, z ).c_str(), std::ios::binary );
		return stream.is_open();
	}
	
	void WorldSerializer::loadChunk( ChunkScalar x, ChunkScalar y, ChunkScalar z )
	{
		// Open the stream.
		std::ifstream stream( resolveRegion( x, y, z ).c_str(), std::ios::binary );
		
		if( !stream.is_open() ) return;
		
		auto c = mWorld->getChunk( x, y, z );
		if( c == nullptr ) 
		{
			c = mWorld->createChunk( x, y, z );
		}
		
		// Read the header.
		size_t types;
		stream.read( (char*)&types, sizeof(size_t) );
		size_t ind = 0;
		std::map<size_t, std::string> tmap;
		for( size_t i = 0; i < types; i++ )
		{
			std::string type;
			std::getline(stream, type, '\0');
			tmap[i+1] = type;
		}
		
		for( int i = 0; i < CHUNK_SIZE; i++ )
		{
			size_t id;
			stream.read( (char*)(&id), sizeof(size_t) );
			if( id != 0 )
			{
				std::string b = tmap[id];
				if( b != "" )
				{
					auto block = FactoryManager::getManager().createBlock(b);
					if( block != nullptr )
						c->setBlockAt( block, i );
				}
			}
		}
		
		stream.close();
	}
	
	void WorldSerializer::saveChunk( ChunkScalar x, ChunkScalar y, ChunkScalar z )
	{
		// Open the stream.
		std::ofstream stream( resolveRegion( x, y, z ).c_str() );//, std::ios::binary );
		
		if( !stream.is_open() ) return;
		
		auto c = mWorld->getChunk( x, y, z );
		if( c == nullptr ) 
		{
			stream.close();
			return;
		}
	
		// Write the header.
		BlockFactoryList& list = FactoryManager::getManager().blockFactoryList;
		size_t types = list.size();
		size_t ind = 0;
		std::map<std::string, size_t> tmap;
		stream.write( (char*)&types, sizeof(size_t) );
		for( BlockFactoryList::iterator it = list.begin(); it != list.end(); ++it )
		{
			auto s = it->first.c_str();
			tmap[it->first] = ++ind;
			stream.write( s, strlen(s) + 1 );
		}
		
		for( int i = 0; i < CHUNK_SIZE; i++ )
		{
			auto b = c->getBlockAt(i);
			if( b != nullptr ) {
				size_t id = tmap[b->getType()];
				stream.write( (char*)&id , sizeof(size_t) );
			}
			else
			{
				size_t null = 0;
				stream.write( (char*)&null, sizeof(size_t) );
			}
		}
		
		stream.close();
	}
};
