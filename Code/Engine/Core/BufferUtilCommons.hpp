#pragma once
#include "Engine/Commons/EngineCommon.hpp"

//------------------------------------------------------------------------------------------------------------------------------
enum eBufferEndianness
{
	BUFFER_NATIVE,
	BUFFER_LITTLE_ENDIAN,
	BUFFER_BIG_ENDIAN
};

//Need to figure this out in runTime and setup in EngineBuildPreferences
#define PLATFORM_IS_BIG_ENDIAN 0
#define PLATFORM_IS_LITTLE_ENDIAN 1

//------------------------------------------------------------------------------------------------------------------------------
typedef std::vector<unsigned char> Buffer;

//Basic buffer utils to handle shuffling bytes based on Endian-ness
void Reverse2BytesInPlace(void* ptrTo16BitWord);		//For short
void Reverse4BytesInPlace(void* ptrTo32BitDword);		//For int, uint, float
void Reverse8BytesInPlace(void* ptrTo64BitQword);		//For double
void ReverseBytesInPlacePtrSizeT(void* ptrToPtrOrSizeT);	//For pointers or Size_t or basically anything that is pointer sized

//------------------------------------------------------------------------------------------------------------------------------
inline void Reverse2BytesInPlace(void* ptrTo16BitWord)
{
	unsigned short u = *(unsigned short*)ptrTo16BitWord;
	*(unsigned short*)ptrTo16BitWord = ((u & 0x00ff) << 8) |
		((u & 0xff00) >> 8);
}


//------------------------------------------------------------------------------------------------------------------------------
inline void Reverse4BytesInPlace(void* ptrTo32BitDword)
{
	unsigned int u = *(unsigned int*)ptrTo32BitDword;
	*(unsigned int*)ptrTo32BitDword = ((u & 0x000000ff) << 24) |
		((u & 0x0000ff00) << 8) |
		((u & 0x00ff0000) >> 8) |
		((u & 0xff000000) >> 24);
}


//------------------------------------------------------------------------------------------------------------------------------
inline void Reverse8BytesInPlace(void* ptrTo64BitQword)
{
	int64_t u = *(int64_t*)ptrTo64BitQword;
	*(int64_t*)ptrTo64BitQword = ((u & 0x00000000000000ff) << 56) |
		((u & 0x000000000000ff00) << 40) |
		((u & 0x0000000000ff0000) << 24) |
		((u & 0x00000000ff000000) << 8) |
		((u & 0x000000ff00000000) >> 8) |
		((u & 0x0000ff0000000000) >> 24) |
		((u & 0x00ff000000000000) >> 40) |
		((u & 0xff00000000000000) >> 56);
}


//------------------------------------------------------------------------------------------------------------------------------
inline void ReverseBytesInPlacePtrSizeT(void* ptrToPtrOrSizeT)
{
	if constexpr (sizeof(void*) == 8)
	{
		Reverse8BytesInPlace(ptrToPtrOrSizeT);
	}
	else
	{
		Reverse4BytesInPlace(ptrToPtrOrSizeT);
	}
}