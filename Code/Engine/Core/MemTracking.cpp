#include "Engine/Core/MemTracking.hpp"
#include "Engine/Core/UntrackedAllocator.hpp"
#include "Engine/Commons/EngineCommon.hpp"
#include "Game/EngineBuildPreferences.hpp"
#include <malloc.h>

std::mutex gTrackerLock;

std::map<void*,		// key
	MemTrackInfo_T, // value
	std::less<void*>, // less to use for map
	UntrackedAllocator<std::pair<void* const, MemTrackInfo_T>> // allocator  
> gMemTrackers;

/*
//------------------------------------------------------------------------------------------------------------------------------
std::mutex& GetMemTrackerLock()
{
	static std::mutex trackerLock;
	return trackerLock;
}

//------------------------------------------------------------------------------------------------------------------------------
std::map<void*,	MemTrackInfo_T,	std::less<void*>, UntrackedAllocator<std::pair<void* const, MemTrackInfo_T>>>& GetMemTrakingMap()
{
	static std::map<void*, MemTrackInfo_T, std::less<void*>, UntrackedAllocator<std::pair<void* const, MemTrackInfo_T>>> memTrackerMap;
	return memTrackerMap;
}
*/

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
		gTotalBytesAllocated += byte_count;
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
	::free(ptr);
	UntrackAllocation(ptr);
#endif
}

//------------------------------------------------------------------------
void TrackAllocation(void* allocation, size_t byte_count)
{
#if (MEM_TRACKING == MEM_TRACK_VERBOSE)
	Callstack callstack = CallstackGet();
	
	MemTrackInfo_T info;
	++gTotalAllocations;
	gTotalBytesAllocated += byte_count;

	info.m_byteSize = byte_count;
	info.m_callstack = callstack;
	info.m_originalPointer = allocation;
	{
		std::scoped_lock lock(gTrackerLock);
		gMemTrackers[allocation] = info;
	}
#endif
}

//------------------------------------------------------------------------
void UntrackAllocation(void* allocation)
{
	{
		std::scoped_lock lock(gTrackerLock);
		auto mapIterator = gMemTrackers.find(allocation);
		if (mapIterator != gMemTrackers.end())
		{
			gTotalBytesAllocated -= mapIterator->second.m_byteSize;
			gMemTrackers.erase(mapIterator);
		}
	}
}

//------------------------------------------------------------------------
size_t MemTrackGetLiveAllocationCount()
{
#if defined(MEM_TRACKING)
	return gTotalAllocations;
#elif
	return 0;
#endif
}

//------------------------------------------------------------------------
size_t MemTrackGetLiveByteCount()
{
#if defined(MEM_TRACKING)
	return gTotalBytesAllocated;
#elif
	return 0;
#endif
}

//------------------------------------------------------------------------
void MemTrackLogLiveAllocations()
{
#if defined(MEM_TRACKING)
	#if (MEM_TRACKING == MEM_TRACK_VERBOSE)
		TODO("Log your allocation data here");
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