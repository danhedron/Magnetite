#include "WorldSerializer.h"
#include <World.h>
#include <BaseBlock.h>
#include <BlockFactory.h>
#include <Profiler.h>

namespace Magnetite
{
	WorldSerializer::WorldSerializer( World* w )
	: mWorld( w )
	{
		mWorldPath = "./worlds/" + mWorld->getName();
	}
	
	WorldSerializer::~WorldSerializer()
	{
		
	}
	
	String WorldSerializer::resolveRegion( ChunkScalar x, ChunkScalar y, ChunkScalar z )
	{
		return mWorldPath + "/chunk" + Util::toString(x) + "." + Util::toString(y) + "." + Util::toString(z);
	}
	
	bool WorldSerializer::hasChunk( ChunkScalar x, ChunkScalar y, ChunkScalar z )
	{
		std::ifstream stream( resolveRegion( x, y, z ).c_str(), std::ios::binary );
		return stream.is_open();
	}
	
	bool WorldSerializer::loadChunk( ChunkScalar x, ChunkScalar y, ChunkScalar z )
	{
		Perf::Profiler::get().begin("streamo");
		
		std::ifstream stream( resolveRegion(x, y, z).c_str() );
		if( !stream.is_open() ) return false;
		
		Perf::Profiler::get().end("streamo");

		auto c = mWorld->getChunk( x, y, z );
		if( c == nullptr ) 
		{
			c = mWorld->createChunk( x, y, z );
		}
		
		// Read the header.
		size_t types;
		size_t blockcount;
		stream.read( (char*)&types, sizeof(size_t) );
		std::string tmap[types];
		for( size_t i = 0; i < types; i++ )
		{
			std::string type;
			std::getline(stream, type, '\0');
			tmap[i] = type;
		}
		
		stream.read( (char*)&blockcount, sizeof(size_t) );
		size_t* data = new size_t[blockcount];
		stream.read( (char*)data, sizeof(size_t)*blockcount );
		
		stream.close();
		
		size_t id;
		for( int i = 0; i < blockcount; i++ )
		{
			id = data[i];
			if( id != 0 )
			{
				auto block = FactoryManager::getManager().createBlock(tmap[id-1]);
				if( block != nullptr )
					c->setBlockAt( block, i );
			}
		}
		
		delete[] data;
		
		return true;
	}
	
	void WorldSerializer::saveChunk( ChunkScalar x, ChunkScalar y, ChunkScalar z )
	{
		std::ofstream stream( resolveRegion(x, y, z).c_str() );
		
		auto c = mWorld->getChunk( x, y, z );
		if( c == nullptr ) 
		{

			stream.close();
			return;
		}
	
		// Write the header.
		BlockFactoryList& list = FactoryManager::getManager().blockFactoryList;
		size_t types = list.size();
		size_t blocks = CHUNK_SIZE;
		size_t ind = 0;
		std::map<std::string, size_t> tmap;
		stream.write( (char*)&types, sizeof(size_t) );
		for( BlockFactoryList::iterator it = list.begin(); it != list.end(); ++it )
		{
			auto s = it->first.c_str();
			tmap[it->first] = ++ind;
			stream.write( s, strlen(s) + 1 );
		}
		stream.write( (char*)(&blocks), sizeof(size_t) );
		
		size_t *data = new size_t[CHUNK_SIZE];
		
		for( int i = 0; i < CHUNK_SIZE; i++ )
		{
			auto b = c->getBlockAt(i);
			if( b != nullptr ) {
				data[i] = tmap[b->getType()];
			}
			else
			{
				data[i] = 0;
			}
		}
		
		stream.write( (char*)data , sizeof(size_t) * CHUNK_SIZE );
		
		delete[] data;
		
		stream.close();
	}
};
