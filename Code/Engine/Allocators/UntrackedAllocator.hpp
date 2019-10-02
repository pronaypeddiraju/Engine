#pragma once
//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Allocators/InternalAllocator.hpp"
#include "Engine/Core/MemTracking.hpp"

//------------------------------------------------------------------------------------------------------------------------------
class UntrackedAllocator : public InternalAllocator
{
public:
	virtual void*					Allocate(size_t size) final;
	virtual void					Free(void* ptr) final;

	static	UntrackedAllocator*		CreateInstance();
	static	void					DestroyInstance();
	static	UntrackedAllocator*		GetInstance();
};

