#include "Profiler.h"
#include <util.h>
#include <sys/time.h>

namespace Perf 
{
	
	Profiler& Profiler::get()
	{
		static Profiler prof;
		return prof;
	}
	
	void Profiler::begin( const std::string& section )
	{
		ProfilerEvents::iterator it = mEvents.find( section );
		if( it != mEvents.end() )
		{
			timeval time;
			gettimeofday(&time, NULL);
			long millis = (time.tv_sec * 1000) + (time.tv_usec / 1000);
			it->second.currentStart = millis;
			return;
		}
		
		ProfileEntry ent;
		timeval time;
		gettimeofday(&time, NULL);
		long millis = (time.tv_sec * 1000) + (time.tv_usec / 1000);
		ent.currentStart = millis;
		ent.latest = 0;
		ent.name = section;
		ent.total = 0;
		
		mMutex.lock();
		mEvents.insert( ProfilerEvents::value_type( section, ent ) );
		mMutex.unlock();
	}
	
	void Profiler::end( const std::string& section )
	{
		mMutex.lock();
		ProfilerEvents::iterator it = mEvents.find( section );
		if( it != mEvents.end() )
		{
			timeval time;
			gettimeofday(&time, NULL);
			long millis = (time.tv_sec * 1000) + (time.tv_usec / 1000);
			it->second.latest = millis - it->second.currentStart;
			it->second.total += it->second.latest;
		}
		mMutex.unlock();
	}
	
	uint32_t Profiler::getLastTime( const std::string& section )
	{
		uint32_t time = 0;
		mMutex.lock();
		ProfilerEvents::iterator it = mEvents.find( section );
		if( it != mEvents.end() )
		{
			time = it->second.latest;
		}
		mMutex.unlock();
		return time;
	}
	
	uint32_t Profiler::getTotalTime( const std::string& section )
	{
		uint32_t time = 0;
		mMutex.lock();
		ProfilerEvents::iterator it = mEvents.find( section );
		if( it != mEvents.end() )
		{
			time = it->second.total;
		}
		mMutex.unlock();
		return time;
	}
};