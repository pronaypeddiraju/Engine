#pragma once
//------------------------------------------------------------------------------------------------------------------------------
#include <thread>
#include <vector>
#include <shared_mutex>
//------------------------------------------------------------------------------------------------------------------------------
typedef unsigned int uint;

//------------------------------------------------------------------------------------------------------------------------------
struct ProfilerSample_T
{
	//Parent Node
	ProfilerSample_T*			m_parent;

	//Node's children
	ProfilerSample_T*			m_lastChild;
	ProfilerSample_T*			m_prevSibling;

	char						m_label[32];

	//Timing
	uint						m_startTime;
	uint						m_endTime;

	std::thread::id				m_threadID;
	uint						m_refCount = 0;

	// memory
	// alloc_count, byte_count
	uint						m_allocCount = 0;
	size_t						m_allocationSizeInBytes = 0;

	void						AddChild();
};

//------------------------------------------------------------------------------------------------------------------------------
static thread_local ProfilerSample_T*			tActiveNode = nullptr;

static std::vector<ProfilerSample_T*>			gHistory;
static std::shared_mutex						gHistoryLock;