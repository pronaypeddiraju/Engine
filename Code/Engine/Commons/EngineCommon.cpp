#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/Time.hpp"
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

//------------------------------------------------------------------------------------------------------------------------------
NamedStrings g_gameConfigBlackboard;

std::atomic<size_t> gTotalAllocations = 0U;
std::atomic<size_t> gTotalBytesAllocated = 0U;

std::atomic<size_t> gAllocatedThisFrame = 0U;
std::atomic<size_t> gAllocatedBytesThisFrame = 0U;

thread_local size_t tTotalAllocations = 0U;
thread_local size_t tTotalBytesAllocated = 0U;

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
