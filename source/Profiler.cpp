#include "Profiler.h"
#include <util.h>
#include <sys/time.h>

namespace Perf 
{
	std::mutex Profiler::profilersMutex;
	
	Profilers& Profiler::getProfilers()
	{
		static Profilers profs;
		return profs;
	}
	
	Profiler& Profiler::get()
	{
		// get the thread id.
		auto tid = std::this_thread::get_id();
		// The profiler ptr.
		ProfilerPtr ptr;
		
		Profilers& profs = getProfilers();
		{
			profilersMutex.lock();
			auto it = profs.find( tid );
			if( it != profs.end() ) 
			{
				ptr = it->second;
			}
			else
			{
				auto p = Profilers::value_type(
					tid,
					std::shared_ptr<Profiler>( new Profiler )
									);
				
				ptr = profs.insert(p).first->second;
			}
			profilersMutex.unlock();
		}
		
		return *ptr.get();
	}
	
	void Profiler::pushFrame()
	{
		mFrames.push_back( mCurrentFrame );
		if( mFrames.size() > FRAME_COUNT )
		{
			mFrames.pop_front();
		}
		mCurrentFrame = ProfilerEvents();
	}
	
	void Profiler::begin( const std::string& section )
	{
		ProfilerEvents::iterator it = mCurrentFrame.find( section );
		if( it != mCurrentFrame.end() )
		{
			timeval time;
			gettimeofday(&time, NULL);
			long millis = (time.tv_sec * 1000) + (time.tv_usec / 1000);
			it->second.start = millis;
			return;
		}
		
		ProfileEntry ent;
		timeval time;
		gettimeofday(&time, NULL);
		long millis = (time.tv_sec * 1000) + (time.tv_usec / 1000);
		ent.start = millis;
		ent.latest = 0;
		ent.name = section;
		ent.total = 0;
		
		mMutex.lock();
		mCurrentFrame.insert( ProfilerEvents::value_type( section, ent ) );
		mMutex.unlock();
	}
	
	void Profiler::end( const std::string& section )
	{
		mMutex.lock();
		ProfilerEvents::iterator it = mCurrentFrame.find( section );
		if( it != mCurrentFrame.end() )
		{
			timeval time;
			gettimeofday(&time, NULL);
			long millis = (time.tv_sec * 1000) + (time.tv_usec / 1000);
			it->second.latest = millis - it->second.start;
			it->second.total += it->second.latest;
		}
		mMutex.unlock();
	}
	
	void Profiler::newFrame()
	{
		pushFrame();
	}
	
	uint32_t Profiler::getLastTime( const std::string& section )
	{
		uint32_t time = 0;
		mMutex.lock();
		ProfilerEvents::iterator it = mCurrentFrame.find( section );
		if( it != mCurrentFrame.end() )
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
		ProfilerEvents::iterator it = mCurrentFrame.find( section );
		if( it != mCurrentFrame.end() )
		{
			time = it->second.total;
		}
		mMutex.unlock();
		return time;
	}
	
	Frames& Profiler::getFrames()
	{
		return mFrames;
	}
};