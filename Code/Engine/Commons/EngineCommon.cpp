#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Core/NamedStrings.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

//------------------------------------------------------------------------------------------------------------------------------
NamedStrings g_gameConfigBlackboard;

std::atomic<size_t> gAllocatedThisFrame = 0U;
std::atomic<size_t> gAllocatedBytesThisFrame = 0U;
std::atomic<size_t> gTotalAllocations = 0U;
std::atomic<size_t> gTotalBytesAllocated = 0U;

//------------------------------------------------------------------------------------------------------------------------------
bool IsBitSet(uint flags, uint bit)
{
	return (flags & (1 << bit)) != 0;
}

//------------------------------------------------------------------------------------------------------------------------------
bool AreAllBitsSet(uint currentFlags, uint flagsToCheck)
{
	return ((currentFlags & flagsToCheck) == flagsToCheck);
}

//------------------------------------------------------------------------------------------------------------------------------
bool AreAnyBitsSet(uint currentFlags, uint flagsToCheck)
{
	return (currentFlags & flagsToCheck);
}

//------------------------------------------------------------------------------------------------------------------------------
uint SetBit(uint flags, uint bit)
{
	flags = flags | (1 << bit);
	return flags;
}

//------------------------------------------------------------------------------------------------------------------------------
uint ClearBit(uint flags, uint bit)
{
	flags = flags & (~(1 << bit));
	return flags;
}

//------------------------------------------------------------------------------------------------------------------------------
uint SetBitTo(uint flags, uint bit, bool set)
{
	if (set)
	{
		flags = SetBit(flags, bit);
	}
	else
	{
		flags = ClearBit(flags, bit);
	}
	return flags;
}

//------------------------------------------------------------------------------------------------------------------------------
// OVERLOADING NEW AND DELETE
//------------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------------------
void* operator new(size_t size)
{
	++gAllocatedThisFrame;
	++gTotalAllocations;

	gAllocatedBytesThisFrame += size;
	gTotalBytesAllocated += size;

	//We are allocating memory for size of size
	//Add sizeof(size_t) to create a slightly larger buffer for metadata
	size_t *buffer = (size_t*) ::malloc(sizeof(size_t) + size);
	//The first part of the memory will now have the size of the entity, the second part is the entity itself
	*buffer = size;
	//Now we are returning the entity part of the memory we allocated
	return buffer + 1;
}

//------------------------------------------------------------------------------------------------------------------------------
void operator delete(void* ptr)
{
	if (ptr == nullptr)
		return;

	--gTotalAllocations;

	//We need to remove the meta data from the front of the allocated memory
	size_t* size_ptr = (size_t*)ptr;
	size_ptr--;

	gTotalBytesAllocated -= *size_ptr;

	//Now that we removed the meta data of size_t, we can now free the memeory allocated
	return ::free(size_ptr);
}

