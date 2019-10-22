#include "Engine/Commons/Profiler/Profiler.hpp"
//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Allocators/BlockAllocator.hpp"
#include "Engine/Allocators/InternalAllocator.hpp"
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Commons/Profiler/ProfilerReport.hpp"
#include "Engine/Core/MemTracking.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/ImGUISystem.hpp"
#include "Engine/Core/WindowContext.hpp"
#include "ThirdParty/imGUI/imgui_internal.h"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/DevConsole.hpp"

//------------------------------------------------------------------------------------------------------------------------------
#include "Game/EngineBuildPreferences.hpp"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

Profiler* gProfiler = nullptr;

//------------------------------------------------------------------------------------------------------------------------------
//Used by Profiler to store nodes with respect to each thread
static thread_local ProfilerSample_T*	tActiveNode = nullptr;
thread_local int tProfilerDepth = 0;

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
	g_eventSystem->SubscribeEventCallBackFn("ProfilerReport", Command_ProfilerReport);

	Profiler* profiler = CreateInstance();
	profiler->ProfilerAllocation(profiler->m_AllowedSize);

	gProfileReporter->CreateInstance();

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
void Profiler::ProfilerPause()
{
	m_isPaused = true;
}

//------------------------------------------------------------------------------------------------------------------------------
void Profiler::ProfilerResume()
{
	m_isPaused = false;
}

//------------------------------------------------------------------------------------------------------------------------------
void Profiler::ProfilerTogglePause()
{
	if (m_isPaused)
	{
		m_isPaused = false;
	}
	else
	{
		m_isPaused = true;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void Profiler::ProfilerPush(const char* label)
{
	ProfilerSample_T* newNode = AllocateNode();

	newNode->m_parent = tActiveNode;
	newNode->m_lastChild = nullptr;
	newNode->m_prevSibling = nullptr;

	newNode->m_threadID = std::this_thread::get_id();

	// setup now
	newNode->m_startTime = GetCurrentTimeHPC();
	strcpy_s(newNode->m_label, label);

	if (tActiveNode != nullptr) 
	{
		tActiveNode->AddChild(newNode);
	}
	else
	{
		if (!m_isPaused && tProfilerDepth == 0)
		{
			tActiveNode = newNode;
		}
	}

	// this is now my active node
	tActiveNode = newNode;

	++tProfilerDepth;
}

//------------------------------------------------------------------------------------------------------------------------------
void Profiler::ProfilerPop()
{
	ASSERT_RECOVERABLE((tProfilerDepth > 0), "The Profiler depth is lesser than or equal to 0");
	--tProfilerDepth;

	if (tActiveNode == nullptr) 
	{
		return;
	}

	// finalize
	tActiveNode->m_endTime = GetCurrentTimeHPC();
	tActiveNode->m_allocCount = (int)tTotalAllocations - tActiveNode->m_allocCount;
	tActiveNode->m_allocationSizeInBytes = tTotalBytesAllocated - tActiveNode->m_allocationSizeInBytes;
	tActiveNode->m_freeCount = (int)tTotalFrees - tActiveNode->m_freeCount;
	tActiveNode->m_freeSizeInBytes = tTotalBytesFreed - tActiveNode->m_freeSizeInBytes;

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

	//Show the profiler window if we have report enabled
	if (m_showTimeline)
	{
		ShowProfilerTimeline();
	}

	EraseOldTrees();
}

//------------------------------------------------------------------------------------------------------------------------------
void Profiler::ShowProfilerTimeline()
{
	if (gProfiler == nullptr || !g_devConsole->IsOpen())
	{
		return;
	}

	//Show the imGUI widget here
	if (g_ImGUI == nullptr)
	{
		ERROR_AND_DIE("The imGUI system was not initialized! The profiler requires you to intialize it");
	}

	
	ImGuiWindowFlags flags = 0;
	flags |= ImGuiWindowFlags_NoInputs;
	flags |= ImGuiWindowFlags_NoMouseInputs;
	
	//Use this place to create/update info for imGui
	ImGui::Begin("Profiler Window", NULL, flags);                          
	ImGui::SetWindowCollapsed(false);
	IntVec2 clientSize = g_windowContext->GetTrueClientBounds();
	ImGui::SetWindowSize(ImVec2((float)clientSize.x, (float)clientSize.y - 50.f), 0);
	ImGui::SetWindowPos(ImVec2(0,0));

	if (!m_isPaused)
	{
		RepopulateReportData();
	}

	MakeTimelineWindow();
	MakeAllocationsWindow();

	ImGui::End();
}


//------------------------------------------------------------------------------------------------------------------------------
void Profiler::PopulateGraphData(float* floatArray, float* allocArray, int& timeArraySize, int& allocArraySize, float& maxTime, float& maxAlloc)
{
	std::vector<ProfilerSample_T*>::iterator itr = m_History.begin();

	while (itr != m_History.end())
	{
		*floatArray = (float)(GetHPCToSeconds((*itr)->m_endTime) - GetHPCToSeconds((*itr)->m_startTime));
		*allocArray = (float)(*itr)->m_allocCount - (float)(*itr)->m_freeCount;

		if (maxTime < *floatArray)
		{
			maxTime = *floatArray;
		}
		if (maxAlloc < *allocArray)
		{
			maxAlloc = *allocArray;
		}

		floatArray++;
		allocArray++;

		timeArraySize++;
		allocArraySize++;

		itr++;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void Profiler::MakeTimelineWindow()
{
	IntVec2 clientSize = g_windowContext->GetTrueClientBounds();

	ImGui::Begin("Timeline Window");
	ImGui::SetWindowPos(ImVec2(50, 50));
	ImVec2 imClientSize = ImVec2((float)clientSize.x - 200.f, 100.f);
	ImGui::PlotHistogram("Timeline view", m_timeArray, m_timeArraySize, 0, "Time taken by each frame", 0, m_maxTime, imClientSize);
	ImVec2 innerRectMax = ImGui::GetItemRectMax();
	ImVec2 innerRectMin = ImGui::GetItemRectMin();
	ImRect innerRect = ImRect(innerRectMin, innerRectMax);

	ImGuiIO& io = ImGui::GetIO();
	if (io.MouseClicked[0])
	{
		//ImGuiID ID = ImGui::GetHoveredID();
		if (innerRect.Contains(io.MousePos))
		{
			const float t = ImClamp((io.MousePos.x - innerRect.Min.x) / (innerRect.Max.x - innerRect.Min.x), 0.0f, 0.9999f);
			const int v_idx = (int)(t * m_timeArraySize);
			IM_ASSERT(v_idx >= 0 && v_idx < m_timeArraySize);

			/*
			if (v_idx != m_reportFrameNum)
			{
				RepopulateReportData();
			}
			*/
			m_reportFrameNum = (float)v_idx;

			ProfilerPause();
		}
	}
	else if (io.MouseClicked[1])
	{
		ProfilerResume();
	}

	if (m_isPaused)
	{
		ProfilerReport* reporter = gProfileReporter->GetInstance();
		if (reporter->GetMode() == FLAT_VIEW)
		{
			reporter->DrawFlatViewAsImGUIWidget((uint)m_reportFrameNum);
		}
		else
		{
			reporter->DrawTreeViewAsImGUIWidget((uint)m_reportFrameNum);
		}
	}

	ImGui::End();
}

//------------------------------------------------------------------------------------------------------------------------------
void Profiler::MakeAllocationsWindow()
{
	IntVec2 clientSize = g_windowContext->GetTrueClientBounds();

	ImGui::Begin("Allocations Window");
	ImGui::SetWindowPos(ImVec2(50, 200));
	ImGui::PlotHistogram("Allocations view", m_allocArray, m_allocArraySize, 0, "Allocations by each frame", 0, m_maxAlloc, ImVec2((float)clientSize.x - 200.f, 100.f));
	
	ImVec2 innerRectMax = ImGui::GetItemRectMax();
	ImVec2 innerRectMin = ImGui::GetItemRectMin();
	ImRect innerRect = ImRect(innerRectMin, innerRectMax);

	ImGuiIO& io = ImGui::GetIO();
	if (io.MouseClicked[0])
	{
		//ImGuiID ID = ImGui::GetHoveredID();
		if (innerRect.Contains(io.MousePos))
		{
			const float t = ImClamp((io.MousePos.x - innerRect.Min.x) / (innerRect.Max.x - innerRect.Min.x), 0.0f, 0.9999f);
			const int v_idx = (int)(t * m_timeArraySize);
			IM_ASSERT(v_idx >= 0 && v_idx < m_timeArraySize);

			/*
			if (v_idx != m_reportFrameNum)
			{
				RepopulateReportData();
			}
			*/
			m_reportFrameNum = (float)v_idx;

			ProfilerPause();
		}
	}
	else if (io.MouseClicked[1])
	{
		ProfilerResume();
	}

	/*
	ImGui::Begin("Allocations Line View Window");
	ImGui::SetWindowPos(ImVec2(50, 350));
	ImGui::PlotLines("Allocations View", allocArray, allocArraySize, 0, "Allocations by each frame", 0, maxAlloc, ImVec2(clientSize.x - 200, 100));
	ImGui::End();
	*/

	ImGui::End();
}

//------------------------------------------------------------------------------------------------------------------------------
void Profiler::EraseOldTrees()
{
	//Check for anything that is older than m_maxHistoryTime and delete if it is

	std::scoped_lock<std::shared_mutex> historyLock(m_HistoryLock);

	int index = 0;

	while (index < m_History.size())
	{
		double currentTime = GetHPCToSeconds(GetCurrentTimeHPC());
		double sampleTime = GetHPCToSeconds(m_History[index]->m_endTime);

		if (sampleTime < currentTime - m_maxHistoryTime)
		{
			ProfilerSample_T* sample = m_History[index];
			ProfilerReleaseTree(sample);
			m_History.erase(m_History.begin() + index);
			index--;
		}

		index++;
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
bool Profiler::IsProfilerOpen()
{
	if (m_showTimeline)
	{
		return true;
	}
	else
	{
		return false;
	}
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
void Profiler::FreeTree(ProfilerSample_T* root)
{
	//Free all children and then free the root
	ProfilerSample_T* node = nullptr;
	ProfilerSample_T* next = nullptr;

	node = root;
	while (node != nullptr)
	{
		if (node->m_lastChild != nullptr)
		{
			next = node->m_lastChild;
			//FreeNode(next);
			node->m_lastChild = nullptr;
		}
		else
		{
			if (node->m_prevSibling != nullptr)
			{
				next = node->m_prevSibling;
				FreeNode(node);
			}
			else
			{
				next = node->m_parent;
				FreeNode(node);
			}
		}
		node = next;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
ProfilerSample_T* Profiler::AllocateNode()
{
	BlockAllocator* instance = gBlockAllocator->GetInstance();
	ProfilerSample_T* node = (ProfilerSample_T*)instance->Allocate(sizeof(ProfilerSample_T));

	if (node != nullptr)
	{
		node->m_allocationSizeInBytes = tTotalBytesAllocated;
		node->m_allocCount = (int)tTotalAllocations;
		
		node->m_freeCount = (int)tTotalFrees;
		node->m_freeSizeInBytes = tTotalBytesFreed;

		node->m_refCount = 1;
	}

	ASSERT_RECOVERABLE(node != nullptr, "Ran out of memory blocks on gBlockAllocator");

	return node;
}

//------------------------------------------------------------------------------------------------------------------------------
void Profiler::FreeNode(ProfilerSample_T* node)
{
	node->m_allocationSizeInBytes = tTotalBytesAllocated - node->m_allocationSizeInBytes;
	node->m_allocCount = (int)tTotalAllocations - node->m_allocCount;

	node->m_freeCount = (int)tTotalFrees - node->m_freeCount;
	node->m_freeSizeInBytes = tTotalBytesFreed - node->m_freeSizeInBytes;

	BlockAllocator* instance = gBlockAllocator->GetInstance();
	instance->Free(node);
}

void Profiler::RepopulateReportData()
{
	//repopulate variables for imGUI
	m_timeArray = new float[m_History.size()];
	m_allocArray = new float[m_History.size()];
	m_timeArrayStart = m_timeArray;
	m_allocArrayStart = m_allocArray;
	m_maxAlloc = 0;
	m_maxTime = 0;
	m_timeArraySize = 0;
	m_allocArraySize = 0;

	PopulateGraphData(m_timeArrayStart, m_allocArrayStart, m_timeArraySize, m_allocArraySize, m_maxTime, m_maxAlloc);
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC bool Profiler::Command_ProfilerReport(EventArgs& args)
{
	UNUSED(args);

	gProfiler->m_showTimeline = !gProfiler->m_showTimeline;
	return true;
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

Profiler*		Profiler::CreateInstance() { return nullptr; }
void			Profiler::DestroyInstance() { return; }
Profiler*		Profiler::GetInstance() { return nullptr; }

bool			Profiler::IsProfilerOpen() { return false; }

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