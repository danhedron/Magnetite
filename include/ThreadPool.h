#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_
#include <vector>
#include <queue>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <functional>
#include <stddef.h>
#include "util.h"

namespace Magnetite {
	/**
	 * Generic ThreadPool class for parallelizable tasks.
	 */
	class ThreadPool {
		
		/**
		* Worker Class
		* 
		* Contains the main loop for polling work.
		*/
		class Worker {
		public:
			Worker( ThreadPool& p ) : pool(p) {};
			void operator()() {
				std::function<void(void)> work;
				while(true) {
					{
						std::unique_lock<std::mutex> lock(pool.mJobListMutex);
						
						while(pool.mContinue && pool.mJobList.empty()) {
							pool.condition.wait(lock);
						}
						
						if(!pool.mContinue)
							return;
						
						work = pool.mJobList.front();
						pool.mJobList.pop();
					}
					
					work();
				}
			}
		private:
			ThreadPool& pool;
		};
		
		typedef std::queue<std::function<void(void)>> JobQueue;
		typedef std::vector<std::thread> ThreadList;
		
		/**
		 * Queue of tasks to be completed.
		 */
		JobQueue mJobList;
		
		/**
		 * List of threads so we can keep track of them
		 */
		ThreadList mThreads;
		
		/**
		 * Max number of threads that can be spawned.
		 */
		size_t mMaxThreads;
		
		/**
		 * Mutex for JobList
		 */
		std::mutex mJobListMutex;
		
		std::condition_variable condition;
		
		/**
		 * Abort switch
		 */
		bool mContinue;
		
	public:
		ThreadPool(size_t maxThreads = 0)
		: mMaxThreads(maxThreads),
		mContinue(true)
		{
			// Initialize some threads, up to min(maxThreads, hardware_concurrency);
			size_t hwThreads = std::thread::hardware_concurrency();
			mMaxThreads = maxThreads == 0 ? hwThreads : maxThreads;
			Util::log("Creating: " + Util::toString(mMaxThreads) + " threads");
			for(size_t i = 0; i < mMaxThreads; ++i) {
				mThreads.push_back(std::thread(Worker(*this)));
			}
		}
		
		~ThreadPool() 
		{
			mContinue = false;
			condition.notify_all();
			
			for(size_t i = 0; i < mThreads.size(); ++i) {
				mThreads[i].join();
			}
		}
		
		void schedule(std::function<void(void)> work) 
		{
			{
				std::unique_lock<std::mutex> lock(mJobListMutex);
				mJobList.push(work);
			}
			condition.notify_one();
		}
		
		void join() 
		{
			while(true) {
				{
					std::unique_lock<std::mutex> lock(mJobListMutex);
					if(mJobList.empty()) return;
					std::this_thread::yield();
				}
			}
		}
	};
	
	typedef ThreadPool SimpleThreadPool;
}

#endif
