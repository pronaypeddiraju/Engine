#include "Engine/Core/MemTracking.hpp"
#include "Engine/Allocators/TemplatedUntrackedAllocator.hpp"
#include "Engine/Commons/EngineCommon.hpp"
#include "Game/EngineBuildPreferences.hpp"
#include <malloc.h>
#include <algorithm>
#include "Async/AsyncQueue.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Commons/UnitTest.hpp"
#include "Engine/Commons/Profiler/ProfileLogScope.hpp"
#include <chrono>
#include <thread>
#include <map>

using namespace std::chrono_literals;

/*
std::mutex gTrackerLock;

std::map<void*,		// key
	MemTrackInfo_T, // value
	std::less<void*>, // less to use for map
	UntrackedAllocator<std::pair<void* const, MemTrackInfo_T>> // allocator  
> gMemTrackers;
*/

//------------------------------------------------------------------------------------------------------------------------------
std::mutex& GetMemTrackerLock()
{
	static std::mutex trackerLock;
	return trackerLock;
}

//------------------------------------------------------------------------------------------------------------------------------
std::map<void*,	MemTrackInfo_T,	std::less<void*>, TemplatedUntrackedAllocator<std::pair<void* const, MemTrackInfo_T>>>& GetMemTrakingMap()
{
	static std::map<void*, MemTrackInfo_T, std::less<void*>, TemplatedUntrackedAllocator<std::pair<void* const, MemTrackInfo_T>>> memTrackerMap;
	return memTrackerMap;
}


//------------------------------------------------------------------------------------------------------------------------------
std::string GetSizeString(size_t byte_count)
{
	uint numBytes = (uint)byte_count;
	std::string returnString = "Size of Allocation: ";

	if (numBytes < 1024)
	{
		returnString += Stringf("%u B", numBytes);
	}
	else if (numBytes > 1024 && numBytes < 1024 * 1024)
	{
		returnString += Stringf("%.3f KiB", (float)numBytes / 1024);
	}
	else if (numBytes > 1024 * 1024 && numBytes < 1024 * 1024 * 1024)
	{
		returnString += Stringf("%.3f MiB", (float)numBytes / (1024 * 1024));
	}
	else if (numBytes > (1024 * 1024 * 1024))
	{
		returnString += Stringf("%.3f GiB", (float)numBytes / (1024 * 1024 * 1024));
	}
	
	return returnString;
}

//------------------------------------------------------------------------------------------------------------------------------
void* UntrackedAlloc(size_t size)
{
	return ::malloc(size);
}

//------------------------------------------------------------------------------------------------------------------------------
void UntrackedFree(void* ptr)
{
	return ::free(ptr);
}

//------------------------------------------------------------------------------------------------------------------------------
void* TrackedAlloc(size_t byte_count)
{
	// One suggestion and example on how to break up this function
	// based on build config; 
#if !defined(MEM_TRACKING)
	return UntrackedAlloc(byte_count);
#else
	#if (MEM_TRACKING == MEM_TRACK_ALLOC_COUNT)
		++gTotalAllocations;

		void* allocation = ::malloc(byte_count);
		return allocation;
	#elif (MEM_TRACKING == MEM_TRACK_VERBOSE)
		++gTotalAllocations;
		gTotalBytesAllocated += byte_count;

		void* allocation = ::malloc(byte_count);
		TrackAllocation(allocation, byte_count);
		return allocation;
	#endif
#endif
}

//------------------------------------------------------------------------
void TrackedFree(void* ptr)
{
#if (MEM_TRACKING == MEM_TRACK_ALLOC_COUNT)
	if (ptr == nullptr)
		return;

	--gTotalAllocations;
	return ::free(ptr);
#elif (MEM_TRACKING == MEM_TRACK_VERBOSE)
	--gTotalAllocations;
	UntrackAllocation(ptr);
#endif
}

//------------------------------------------------------------------------
void TrackAllocation(void* allocation, size_t byte_count)
{
#if (MEM_TRACKING == MEM_TRACK_VERBOSE)
	Callstack callstack = CallstackGet();
	
	MemTrackInfo_T info;

	info.m_byteSize = byte_count;
	info.m_callstack = callstack;
	info.m_originalPointer = allocation;
	{
		std::scoped_lock lock(GetMemTrackerLock());
		GetMemTrakingMap()[allocation] = info;
	}
#endif
}

//------------------------------------------------------------------------
void UntrackAllocation(void* allocation)
{
	{
		std::scoped_lock lock(GetMemTrackerLock());
		auto mapIterator = GetMemTrakingMap().find(allocation);
		::free(allocation);
		if (mapIterator != GetMemTrakingMap().end())
		{
			gTotalBytesAllocated -= mapIterator->second.m_byteSize;
			GetMemTrakingMap().erase(mapIterator);
		}
	}
}

//------------------------------------------------------------------------
size_t MemTrackGetLiveAllocationCount()
{
#if defined(MEM_TRACKING)
	return gTotalAllocations;
#else
	return 0;
#endif
}

//------------------------------------------------------------------------
size_t MemTrackGetLiveByteCount()
{
#if defined(MEM_TRACKING)
	return gTotalBytesAllocated;
#else
	return 0;
#endif
}

//------------------------------------------------------------------------------------------------------------------------------
bool MemVecSortFunction(LogTrackInfo_T const& a, LogTrackInfo_T const& b)
{
	return (a.m_numAllocations > b.m_numAllocations);
}

//------------------------------------------------------------------------------------------------------------------------------
void MemTrackLogLiveAllocations()
{
#if defined(MEM_TRACKING)
	#if (MEM_TRACKING == MEM_TRACK_VERBOSE)
		//Create a thread safe copy of the map but use callstack as key and pair of num allocations and alloc size as value

		std::map<unsigned long, LogTrackInfo_T, std::less<unsigned long>, TemplatedUntrackedAllocator<std::pair<unsigned long const, LogTrackInfo_T>>> memLoggerMap;

		std::map<unsigned long, LogTrackInfo_T, std::less<unsigned long>, TemplatedUntrackedAllocator<std::pair<unsigned long const, LogTrackInfo_T>>>::iterator memLoggerIterator;
		std::map<void*, MemTrackInfo_T, std::less<void*>, TemplatedUntrackedAllocator<std::pair<void* const, MemTrackInfo_T>>>::iterator memTrackerIterator;

		size_t totalAllocationSize = 0;
		uint totalAllocations = 0;

		memTrackerIterator = GetMemTrakingMap().begin();
		{
			std::scoped_lock lock(GetMemTrackerLock());
			while (memTrackerIterator != GetMemTrakingMap().end())
			{
				memLoggerIterator = memLoggerMap.find(memTrackerIterator->second.m_callstack.m_hash);
				if (memLoggerIterator == memLoggerMap.end())
				{
					//This hash doesn't exist in the map
					LogTrackInfo_T info;
					info.m_allocationSizeInBytes = memTrackerIterator->second.m_byteSize;
					info.m_numAllocations = 1;
					info.m_callstack = memTrackerIterator->second.m_callstack;

					memLoggerMap[memTrackerIterator->second.m_callstack.m_hash] = info;
				}
				else
				{
					//Hash exists in map so update data (except the callstack as it will be the same)
					memLoggerIterator->second.m_allocationSizeInBytes += memTrackerIterator->second.m_byteSize;
					++memLoggerIterator->second.m_numAllocations;
				}

				totalAllocationSize += memTrackerIterator->second.m_byteSize;
				totalAllocations++;
				memTrackerIterator++;
			}
		}


		//Sort Map
		std::vector<LogTrackInfo_T> logVector;

		std::map<unsigned long, LogTrackInfo_T, std::less<unsigned long>, TemplatedUntrackedAllocator<std::pair<unsigned long const, LogTrackInfo_T>>>::iterator logMapItr;
		logMapItr = memLoggerMap.begin();

		while (logMapItr != memLoggerMap.end())
		{
			logVector.emplace_back(logMapItr->second);
			logMapItr++;
		}

		std::sort(logVector.begin(), logVector.end(), MemVecSortFunction);

		//Log all the elements in the vector
		DebuggerPrintf("===== BEGIN MEMORY LOG =====");
		DebuggerPrintf("\n Total Allocations live: %u", totalAllocations);
		std::string bytesAllocated = GetSizeString(totalAllocationSize);
		DebuggerPrintf("\n %s \n", bytesAllocated.c_str());
		
		std::vector<LogTrackInfo_T>::iterator logVecItr = logVector.begin();
		while (logVecItr != logVector.end())
		{
			DebuggerPrintf("\n Num allocations for hash: %u", logVecItr->m_numAllocations);
			bytesAllocated = GetSizeString(logVecItr->m_allocationSizeInBytes);
			DebuggerPrintf("\n %s \n", bytesAllocated.c_str());
			std::vector<std::string> callStackString = GetCallstackToString(logVecItr->m_callstack);

			logVecItr++;
		}

		DebuggerPrintf("===== END MEMORY LOG =====");

	#endif
#endif
}

//------------------------------------------------------------------------------------------------------------------------------
void* operator new(size_t size)
{
	return TrackedAlloc(size);
}

//------------------------------------------------------------------------------------------------------------------------------
void operator delete(void* ptr)
{
	TrackedFree(ptr);
}

//------------------------------------------------------------------------------------------------------------------------------
// UNIT TEST
//------------------------------------------------------------------------------------------------------------------------------
#if defined(MEM_TRACKING)
#define MEMTEST_ITER_PER_THREAD 1'000'000
#define MEMTEST_ALLOC_BYTE_SIZE 128

static void AllocTest(AsyncQueue<void*>& mem_queue, std::atomic<uint>& running_count)
{
	for (uint i = 0; i < MEMTEST_ITER_PER_THREAD; ++i) 
	{
		// (Random01() > .5f) or however your random functions look
		if (g_RNG->GetRandomFloatZeroToOne() > 0.5f)
		{
			char* ptr = (char*)TrackedAlloc(MEMTEST_ALLOC_BYTE_SIZE);

			// just doing this to slow it down
			// (and later, to confirm memory didn't get currupted)
			for (uint j = 0; j < MEMTEST_ALLOC_BYTE_SIZE; ++j) {
				ptr[j] = (char)j;
			}

			mem_queue.EnqueueLocked(ptr);
		}
		else {
			void* ptr;
			if (mem_queue.DequeueLocked(&ptr)) {
				TrackedFree(ptr);
			}
		}
	}

	// we're done; 
	--running_count;
}

// This test will only work if memory tracking is enabled
// otherwise the memory tracking just return 0;
UNITTEST("A02", nullptr, 100)
{
	// unittest assumes 
	uint pre_allocations = (uint)MemTrackGetLiveAllocationCount();
	uint pre_alloc_bytes = (uint)MemTrackGetLiveByteCount();

	{
		PROFILE_LOG_SCOPE("A02 Test");
		// scope so queue goes out of scope and we
		// get those allocations back; 
		uint core_count = std::thread::hardware_concurrency();
		AsyncQueue<void*> mem_queue;
		std::atomic<uint> live_count = core_count;

		for (unsigned int i = 0; i < core_count; ++i) {
			std::thread test_thread(AllocTest, std::ref(mem_queue), std::ref(live_count));
			test_thread.detach();
		}

		while (live_count.load() > 0) 
		{
			// "ms" is a C++ custom literal equivalent 
			// for std::chrono::milliseconds(100)
			// https://en.cppreference.com/w/cpp/chrono/operator%22%22ms
			std::this_thread::sleep_for(100ms);
		}

		void* ptr;
		while (mem_queue.DequeueLocked(&ptr)) {
			TrackedFree(ptr);
		}
	}

	// check we're back to where we started; 
	uint post_allocations = (uint)MemTrackGetLiveAllocationCount();
	uint post_alloc_bytes = (uint)MemTrackGetLiveByteCount();

	// if done right, allocations at the start
	// should be allocations at the end; 
	return ((pre_allocations == post_allocations) && (pre_alloc_bytes == post_alloc_bytes));
}
#endif
