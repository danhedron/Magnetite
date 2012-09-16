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
		
		mEvents.insert( ProfilerEvents::value_type( section, ent ) );
	}
	
	void Profiler::end( const std::string& section )
	{
		ProfilerEvents::iterator it = mEvents.find( section );
		if( it != mEvents.end() )
		{
			timeval time;
			gettimeofday(&time, NULL);
			long millis = (time.tv_sec * 1000) + (time.tv_usec / 1000);
			it->second.latest = millis - it->second.currentStart;
			it->second.total += it->second.latest;
		}
	}
	
	ProfileEntry& Profiler::getEntry( const std::string& section )
	{
		ProfilerEvents::iterator it = mEvents.find( section );
		if( it != mEvents.end() )
		{
			return it->second;
		}
		// what to do if there is no entry?
	}
};