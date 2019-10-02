#include "Engine/Allocators/UntrackedAllocator.hpp"
//------------------------------------------------------------------------------------------------------------------------------
static UntrackedAllocator* gUntrackedAllocator = nullptr;

//------------------------------------------------------------------------------------------------------------------------------
void* UntrackedAllocator::Allocate(size_t size)
{
	return UntrackedAlloc(size);
}

//------------------------------------------------------------------------------------------------------------------------------
void UntrackedAllocator::Free(void* ptr)
{
	return UntrackedFree(ptr);
}

//------------------------------------------------------------------------------------------------------------------------------
UntrackedAllocator* UntrackedAllocator::CreateInstance()
{
	if (gUntrackedAllocator == nullptr)
	{
		gUntrackedAllocator = new UntrackedAllocator();
	}

	return gUntrackedAllocator;
}

//------------------------------------------------------------------------------------------------------------------------------
void UntrackedAllocator::DestroyInstance()
{
	if(gUntrackedAllocator != nullptr)
	{
		delete gUntrackedAllocator;
		gUntrackedAllocator = nullptr;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
UntrackedAllocator* UntrackedAllocator::GetInstance()
{
	if (gUntrackedAllocator == nullptr)
	{
		CreateInstance();
	}

	return gUntrackedAllocator;
}
