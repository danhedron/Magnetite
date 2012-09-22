#ifndef _PROFILER_H_
#define _PROFILER_H_
#include <map>
#include <string>
#include <mutex>
#include <deque>
#include <thread>
#include <thread>

#define FRAME_COUNT 128

namespace Perf {
	
	struct ProfileEntry
	{
		std::string name;
		uint32_t start;
		uint32_t latest;
		uint32_t total;
	};
	
	class Profiler;
	typedef std::shared_ptr<Profiler> ProfilerPtr;
	typedef std::map<std::thread::id, ProfilerPtr> Profilers;
	
	typedef std::map<std::string,ProfileEntry> ProfilerEvents;
	typedef std::deque<ProfilerEvents> Frames;

	class Profiler
	{
		Frames mFrames;
		ProfilerEvents mCurrentFrame;
		
		// Mutex to ensure we don't cause a crash.
		std::mutex mMutex;
		
		void pushFrame();
	public:
		
		/**
		 * Begins profiling a new block
		 */
		void begin( const std::string& section );
		
		/**
		 * Ends a profiler block
		 */
		void end( const std::string& section );
		
		/**
		 * Tells the profiler that a new frame has started.
		 */
		void newFrame();
		
		/**
		 * Returns the time of the last named block
		 */
		uint32_t getLastTime( const std::string& section );
		
		/**
		 * Returns the sum of all of the named blocks (per frame).
		 */
		uint32_t getTotalTime( const std::string& section );
		
		/**
		 * Returns the internal frame reference, for use in the renderer.
		 */
		Frames& getFrames();
		
		static std::mutex profilersMutex;
		
		static Profiler& get();
		
		static Profilers& getProfilers();

	};

}

#endif