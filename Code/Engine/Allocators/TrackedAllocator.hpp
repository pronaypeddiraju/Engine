#pragma once
//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Allocators/InternalAllocator.hpp"
#include "Engine/Core/MemTracking.hpp"

//------------------------------------------------------------------------------------------------------------------------------
class TrackedAllocator : public InternalAllocator
{
public:
	virtual void*				Allocate(size_t size) final;
	virtual void				Free(void* ptr) final;

	static	TrackedAllocator*						CreateInstance();
	static	void									DestroyInstance();
	static	TrackedAllocator*						GetInstance();
};
