#include "WorldSerializer.h"
#include <World.h>
#include <BaseBlock.h>
#include <BlockFactory.h>
#include <Profiler.h>
#include <Chunk.h>

namespace Magnetite
{
	struct ChunkHeader
	{
		uint32_t tCount;
	};
	
	struct ChunkData
	{
		uint32_t blockData[CHUNK_SIZE];
		LightIndex lightData[CHUNK_SIZE];
		
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
		return  "./worlds/" + mWorld->getName() + "/chunk" + Util::toString(x) + "." + Util::toString(y) + "." + Util::toString(z);
	}
	
	bool WorldSerializer::hasChunk( ChunkScalar x, ChunkScalar y, ChunkScalar z )
	{
		std::ifstream stream( resolveRegion( x, y, z ).c_str(), std::ios::binary );
		return stream.is_open();
	}
	
	bool WorldSerializer::loadChunk( ChunkScalar x, ChunkScalar y, ChunkScalar z )
	{
		std::ifstream stream( resolveRegion(x, y, z).c_str() );
		if( !stream.is_open() ) return false;
		
		auto c = mWorld->getChunk( x, y, z );
		if( c == nullptr ) 
		{
			c = mWorld->createChunk( x, y, z );
		}
		
		ChunkData d;
		stream.read( (char*)&d, sizeof(ChunkData) );
		
		stream.close();
		
		Perf::Profiler::get().begin("dread");
		size_t id;
		LightIndex lv;
		for( int i = 0; i < CHUNK_SIZE; i++ )
		{
			id = d.blockData[i];
			lv = d.lightData[i];
			if( id != 0 )
			{
				auto block = FactoryManager::getManager().createBlock(idmap[id]);
				if( block != nullptr )
					c->setBlockAt( block, i );
			}
			c->setLightLevel( lv, i );
		}
		
		c->_raiseChunkFlag( Chunk::SkipLight );
		
		Perf::Profiler::get().end("dread");
		
		return true;
	}
	
	void WorldSerializer::saveChunk( ChunkScalar x, ChunkScalar y, ChunkScalar z )
	{
		auto c = mWorld->getChunk( x, y, z );
		if( c == nullptr || c->getBlockCount() == 0 ) 
		{
			return;
		}
	
		std::ofstream stream( resolveRegion(x, y, z).c_str() );
		
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
			d.lightData[i] = c->getLightLevel( i );
		}
		
		stream.write( (char*)&d , sizeof(d) );
		
		stream.close();
	}
};
