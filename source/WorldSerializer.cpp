#include "WorldSerializer.h"
#include <World.h>
#include <BaseBlock.h>
#include <BlockFactory.h>
#include <Profiler.h>

namespace Magnetite
{
	struct ChunkHeader
	{
		uint32_t tCount;
	};
	
	struct ChunkData
	{
		uint32_t blockData[CHUNK_SIZE];
	};
	
	static std::map<Magnetite::String, size_t> tmap;
	static std::map<size_t,Magnetite::String> idmap;
	
	WorldSerializer::WorldSerializer( World* w )
	: mWorld( w )
	{
		mWorldPath = "./worlds/" + mWorld->getName();
		
		// Prepare the type proxy list
		BlockFactoryList& list = FactoryManager::getManager().blockFactoryList;
		size_t types = list.size();
		
		size_t ind = 0;

		for( BlockFactoryList::iterator it = list.begin(); it != list.end(); ++it )
		{
			ind++;
			auto s = it->first.c_str();
			tmap[it->first] = ind;
			idmap[ind] = it->first;
		}
		
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
		
		Perf::Profiler::get().begin("sopen");
		std::ifstream stream( resolveRegion(x, y, z).c_str() );
		if( !stream.is_open() ) return false;
		
		Perf::Profiler::get().end("sopen");

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
		
		Perf::Profiler::get().begin("dread");
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
		Perf::Profiler::get().end("dread");
		
		delete[] data;
		
		return true;
	}
	
	void WorldSerializer::saveChunk( ChunkScalar x, ChunkScalar y, ChunkScalar z )
	{
		auto c = mWorld->getChunk( x, y, z );
		if( c == nullptr || c->getBlockCount() == 0 ) 
		{
			return;
		}
	
		//std::ofstream stream( resolveRegion(x, y, z).c_str() );
		
		ChunkData d;
		
		for( int i = 0; i < CHUNK_SIZE; i++ )
		{
			auto b = c->getBlockAt(i);
			if( b != nullptr ) {
				d.blockData[i] = tmap[b->getType()];
			}
			else
			{
				d.blockData[i] = 0;
			}
		}
		
		//stream.write( (char*)&d , sizeof(d) );
		
		///stream.close();
	}
};
