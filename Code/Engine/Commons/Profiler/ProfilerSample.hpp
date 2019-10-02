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
	ProfilerSample_T*			m_parent = nullptr;

	//Node's children
	ProfilerSample_T*			m_lastChild = nullptr;
	ProfilerSample_T*			m_prevSibling = nullptr;

	char						m_label[32];

	//Timing
	uint64_t					m_startTime = 0;
	uint64_t					m_endTime = 0;

	std::thread::id				m_threadID;
	uint						m_refCount = 0;

	// memory
	// alloc_count, byte_count
	uint64_t					m_allocCount = 0;
	size_t						m_allocationSizeInBytes = 0;

	void						AddChild(ProfilerSample_T* child);
};