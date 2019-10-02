#include "Engine/Commons/Profiler/ProfilerSample.hpp"
//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Allocators/BlockAllocator.hpp"
#include "Engine/Commons/EngineCommon.hpp"

//------------------------------------------------------------------------------------------------------------------------------
void ProfilerSample_T::AddChild(ProfilerSample_T* child)
{
	child->m_parent = this;
	child->m_prevSibling = m_lastChild;
	m_lastChild = child;
}
