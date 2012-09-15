#ifndef _PROFILER_H_
#define _PROFILER_H_
#include <map>
#include <string>

namespace Perf {
	
	struct ProfileEntry
	{
		std::string name;
		uint32_t total;
		uint32_t currentStart;
		uint32_t latest;
	};
	
	typedef std::map<std::string,ProfileEntry> ProfilerEvents;
	
	class Profiler
	{
		ProfilerEvents mEvents;
		
	public:
		void begin( const std::string& section );
		
		void end( const std::string& section );
		
		ProfileEntry& getEntry( const std::string& section );
		
		static Profiler& get();
	};

}

#endif