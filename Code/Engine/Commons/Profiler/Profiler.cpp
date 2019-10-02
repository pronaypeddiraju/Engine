#include "Engine/Commons/Profiler/Profiler.hpp"
//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Allocators/BlockAllocator.hpp"
#include "Engine/Allocators/InternalAllocator.hpp"
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Core/MemTracking.hpp"
#include "Engine/Core/Time.hpp"

//------------------------------------------------------------------------------------------------------------------------------
#include "Game/EngineBuildPreferences.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

Profiler* gProfiler = nullptr;

thread_local ProfilerSample_T* Profiler::tActiveNode = nullptr;

#if defined(PROFILING_ENABLED)
//------------------------------------------------------------------------------------------------------------------------------
Profiler::Profiler()
{

}

//------------------------------------------------------------------------------------------------------------------------------
Profiler::~Profiler()
{

}

//------------------------------------------------------------------------------------------------------------------------------
bool Profiler::ProfilerInitialize()
{
	Profiler* profiler = CreateInstance();
	profiler->ProfilerAllocation(profiler->m_AllowedSize);

	return true;
}

//------------------------------------------------------------------------------------------------------------------------------
void Profiler::ProfilerShutdown()
{
	ProfilerFree();
	return DestroyInstance();
}

//------------------------------------------------------------------------------------------------------------------------------
void Profiler::ProfilerSetMaxHistoryTime(double seconds)
{
	m_maxHistoryTime = seconds;
}

//------------------------------------------------------------------------------------------------------------------------------
void Profiler::ProfilerPush(const char* label)
{
	ProfilerSample_T* newNode = AllocateNode();

	newNode->m_parent = tActiveNode;
	newNode->m_threadID = std::this_thread::get_id();

	// setup now
	newNode->m_startTime = GetCurrentTimeHPC();
	strcpy_s(newNode->m_label, label);

	if (tActiveNode != nullptr) 
	{
		tActiveNode->AddChild(newNode);
	}

	// this is now my active node
	tActiveNode = newNode;

	//DebuggerPrintf("\n Frame Pushed %s", label);
}

//------------------------------------------------------------------------------------------------------------------------------
void Profiler::ProfilerPop()
{
	if (tActiveNode == nullptr) 
	{
		return;
	}

	// finalize
	tActiveNode->m_endTime = GetCurrentTimeHPC();

	//Setup where we are now after poping the active node
	ProfilerSample_T* oldActive = tActiveNode;
	tActiveNode = tActiveNode->m_parent;
	if (tActiveNode == nullptr) 
	{
		std::scoped_lock<std::shared_mutex> lock(m_HistoryLock);
		m_History.push_back(oldActive);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void Profiler::ProfilerUpdate()
{
	if (gProfiler == nullptr)
		return;

	//Check for anything that is older than m_maxHistoryTime and delete if it is
	//TODO("Delete old trees");

	std::scoped_lock<std::shared_mutex> historyLock(m_HistoryLock);

	std::vector<ProfilerSample_T*>::iterator historyItr;
	historyItr = m_History.begin();

	while (historyItr != m_History.end())
	{
		double currentTime = GetCurrentTimeSeconds();
		double sampleTime = GetHPCToSeconds((*historyItr)->m_endTime);
		if (sampleTime < currentTime - m_maxHistoryTime)
		{
			m_History.erase(historyItr);
		}

		historyItr++;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void Profiler::ProfilerAllocation(size_t byteSize /*= 0*/)
{
	BlockAllocator* blockAllocator = gBlockAllocator->GetInstance();
	void* buffer = UntrackedAlloc(byteSize);
	blockAllocator->Initialize(buffer, byteSize, sizeof(ProfilerSample_T), alignof(ProfilerSample_T));
}

//------------------------------------------------------------------------------------------------------------------------------
void Profiler::ProfilerFree()
{
	BlockAllocator* blockAllocator = gBlockAllocator->GetInstance();
	blockAllocator->Deinitialize();
}

//------------------------------------------------------------------------------------------------------------------------------
void Profiler::ProfilerBeginFrame(const char* label /*= "Frame"*/)
{
	ASSERT_RECOVERABLE(tActiveNode == nullptr, "The active node was nullptr in ProfilerBeginFrame");

	ProfilerPush(label);
}

//------------------------------------------------------------------------------------------------------------------------------
void Profiler::ProfilerEndFrame()
{
	ProfilerPop();

	ASSERT_RECOVERABLE(tActiveNode == nullptr, "The active node was nullptr in ProfilerEndFrame")
}

//------------------------------------------------------------------------------------------------------------------------------
ProfilerSample_T* Profiler::ProfilerAcquirePreviousTree(std::thread::id id, uint history /*= 0*/)
{
	std::scoped_lock<std::shared_mutex> historyLock(m_HistoryLock);
	//Go back in the vector by "history" frames and acquire frame tree with threadID = id
	uint indexForThread = 0;

	std::vector<ProfilerSample_T*>::iterator itr = m_History.end();
	itr--;

	while (indexForThread != history || itr != m_History.begin())
	{
		if ((*itr)->m_threadID == id)
		{
			indexForThread++;
		}

		itr--;
	}

	if (indexForThread == history)
	{
		return m_History[indexForThread];
	}
	else
	{
		ERROR_RECOVERABLE("Frame history does not exist for specified thread");
		return nullptr;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
ProfilerSample_T* Profiler::ProfilerAcquirePreviousTreeForCallingThread(uint history /*= 0*/)
{
	std::scoped_lock<std::shared_mutex> historyLock(m_HistoryLock);

	int index = (int)m_History.size() - 1;
	index -= history;
	if (index > 0)
	{
		//Frame exists
		return m_History[index];
	}
	else
	{
		//Frame doesn't exist
		ERROR_RECOVERABLE("Frame history does not exist for this thread");
		return nullptr;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void Profiler::ProfilerReleaseTree(ProfilerSample_T* node)
{
	//Atomic decrement operation provided by windows for thread safe decrement
	uint newCount = ::InterlockedDecrement(&node->m_refCount);

	if (newCount == 0)
	{
		FreeTree(node);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC Profiler* Profiler::CreateInstance()
{
	if (gProfiler == nullptr)
	{
		gProfiler = new Profiler();
	}

	return gProfiler;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC void Profiler::DestroyInstance()
{
	if (gProfiler != nullptr)
	{
		delete gProfiler;
		gProfiler = nullptr;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC Profiler* Profiler::GetInstance()
{
	if (gProfiler == nullptr)
	{
		CreateInstance();
	}

	return gProfiler;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC void Profiler::FreeTree(ProfilerSample_T* root)
{
	//Free all children and then free the root
	ProfilerSample_T* node = nullptr;
	ProfilerSample_T* next = nullptr;

	node = root;
	while (node->m_refCount != 0)
	{
		if (node->m_lastChild->m_refCount != 0)
		{
			next = node->m_lastChild;
			FreeNode(node->m_lastChild);
		}
		else
		{
			if (node->m_prevSibling->m_refCount != 0)
			{
				next = node->m_prevSibling;
			}
			else
			{
				next = node->m_parent;
			}
			FreeNode(node);
		}
		node = next;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC ProfilerSample_T* Profiler::AllocateNode()
{
	BlockAllocator* instance = gBlockAllocator->GetInstance();
	ProfilerSample_T* node = (ProfilerSample_T*)instance->Allocate(sizeof(ProfilerSample_T));

	if (node != nullptr)
	{
		node->m_allocationSizeInBytes = tTotalBytesAllocated;
		node->m_allocCount = tTotalAllocations;
		node->m_refCount = 1;
	}

	ASSERT_RECOVERABLE(node != nullptr, "Ran out of memory blocks on gBlockAllocator");

	return node;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC void Profiler::FreeNode(ProfilerSample_T* node)
{
	node->m_allocationSizeInBytes = tTotalBytesAllocated - node->m_allocationSizeInBytes;
	node->m_allocCount = tTotalAllocations - node->m_allocCount;

	BlockAllocator* instance = gBlockAllocator->GetInstance();
	instance->Free(node);
}

#else
bool			Profiler::ProfilerInitialize() { return false; };
void			Profiler::ProfilerShutdown() {};

void			Profiler::ProfilerSetMaxHistoryTime(double seconds) { UNUSED(seconds); };

void			Profiler::ProfilerPause() {};
void			Profiler::ProfilerResume() {};

void			Profiler::ProfilerPush(const char* label) { UNUSED(label); };
void			Profiler::ProfilerPop() {};

void			Profiler::ProfilerUpdate() {};
				
void			Profiler::ProfilerAllocation(size_t byteSize) { UNUSED(byteSize); };
void			Profiler::ProfilerFree() {};
				
void			Profiler::ProfilerBeginFrame(const char* label) { UNUSED(label); };
void			Profiler::ProfilerEndFrame() {};

// We can only really 'view' a complete tree
// these functions return the most recently finished tree
// use a shared_mutex for accessing trees from the system (as it will try to destroy old ones constantly)
// `history` is how many frames back we should try to get
ProfilerSample_T*			ProfilerAcquirePreviousTree(std::thread::id id, uint history = 0) 
{
	UNUSED(id);
	UNUSED(history);
	return nullptr; 
}

ProfilerSample_T*			ProfilerAcquirePreviousTreeForCallingThread(uint history = 0) 
{
	UNUSED(history);
	return nullptr; 
}

void						ProfilerReleaseTree(ProfilerSample_T* node) 
{
	UNUSED(node);
}
#endif