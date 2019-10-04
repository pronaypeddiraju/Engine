#include "Engine/Allocators/TrackedAllocator.hpp"
//------------------------------------------------------------------------------------------------------------------------------
static TrackedAllocator*	gTrackedAllocator = nullptr;

//------------------------------------------------------------------------------------------------------------------------------
void* TrackedAllocator::Allocate(size_t size)
{
	return TrackedAlloc(size);
}

//------------------------------------------------------------------------------------------------------------------------------
void TrackedAllocator::Free(void* ptr)
{
	return TrackedFree(ptr);
}

//------------------------------------------------------------------------------------------------------------------------------
TrackedAllocator* TrackedAllocator::CreateInstance()
{
	if (gTrackedAllocator == nullptr)
	{
		gTrackedAllocator = new TrackedAllocator();
	}

	return gTrackedAllocator;
}

//------------------------------------------------------------------------------------------------------------------------------
void TrackedAllocator::DestroyInstance()
{
	if (gTrackedAllocator != nullptr)
	{
		delete gTrackedAllocator;
		gTrackedAllocator = nullptr;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
TrackedAllocator* TrackedAllocator::GetInstance()
{
	if (gTrackedAllocator == nullptr)
	{
		CreateInstance();
	}

	return gTrackedAllocator;
}
