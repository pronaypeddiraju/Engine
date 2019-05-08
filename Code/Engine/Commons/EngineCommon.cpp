#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Core/NamedStrings.hpp"

//------------------------------------------------------------------------------------------------------------------------------
NamedStrings g_gameConfigBlackboard;

//------------------------------------------------------------------------------------------------------------------------------
bool IsBitSet(uint flags, uint bit)
{
	return (flags & (1 << bit)) != 0;
}

//------------------------------------------------------------------------------------------------------------------------------
bool AreAllBitsSet(uint currentFlags, uint flagsToCheck)
{
	if (currentFlags & flagsToCheck)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
bool AreAnyBitsSet(uint currentFlags, uint flagsToCheck)
{
	if (currentFlags | flagsToCheck)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
uint SetBit(uint flags, uint bit)
{
	flags |= 1 << bit;
	return flags;
}

//------------------------------------------------------------------------------------------------------------------------------
uint ClearBit(uint flags, uint bit)
{
	flags &= ~(1 << bit);
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

