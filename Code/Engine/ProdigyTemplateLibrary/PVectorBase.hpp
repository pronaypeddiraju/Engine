#pragma once
//------------------------------------------------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////

//Intent for PVectorBase is to be the base class that handles the memory 
//allocations required for the PVector class, this makes exception handling
//simpler. If user makes a PVector, the memory allocation for said PVector
//will be handled from PVectorBase. If exception occurs in the allocator 
//during memory allocation, then the exception thrown is jumped 
//to the user (No need to try/catch seperately). If there is an exception
//in PVector, the memory is freed by calling the destructor of PVectorBase 
//before the execution jumps back to user.

//Advantage:
//If the PVector class had to do the initialization and allocation of memory, 
//then we would have to write try/catch blocks for all the pathways that
//allocate memory. This sucks because it does these things:
//	1. Increases Code size
//	2. Decreases performance (Try/catch sucks like that)
//	3. Makes code harder to read and maintain (This I don't like)

//////////////////////////////////////////////////////////////////////////

#include "Engine/Allocators/UntrackedAllocator.hpp"

//------------------------------------------------------------------------------------------------------------------------------
template <typename T, typename PAllocator>
struct PVectorBase
{
	typedef ptrdiff_t		differenceType;

	enum : size_t
	{
		nonValidPos		= -1,	//non valid position.
		maxSize			= -2	//-1 reserved for nonValidPos, maxSize conveniently can be -2 so that we don't have any integer wrap around issues
	};

protected:

	T*			m_begin;
	T*			m_end;

	std::pair<T*, PAllocator>	m_capacityAllocator;

	T*&					GetSetableInternalCapacityPtr()		noexcept
	{
		return m_capacityAllocator.first;
	}

	T* const&			GetInternalCapacityPtr()			noexcept
	{
		return m_capacityAllocator.first;
	}

	PAllocator&			GetSetableInternalAllocator()		noexcept
	{
		return m_capacityAllocator.second;
	}

	const PAllocator&	GetInternalAllocator()				noexcept
	{
		return m_capacityAllocator.second;
	}

public:
	PVectorBase();
	PVectorBase(const PAllocator& allocator);
	PVectorBase(size_t numObjectsToAllocate, const PAllocator& allocator);

	~PVectorBase();

	const PAllocator&		GetAllocator() const noexcept;
	PAllocator&				GetSetableAllocator() noexcept;
	void					SetAllocator(const PAllocator& allocator);

protected:
	T*						Allocate(size_t numObjectsToAllocate);
	void					Free(T* p, size_t numObjectsToFree);
	size_t					GetNewCapacity(size_t currentCapacity);
};


template <typename T, typename PAllocator>
inline PVectorBase<T, PAllocator>::PVectorBase()
	: m_begin(NULL),
	m_end(NULL),
	m_capacityAllocator(NULL, UntrackedAllocator)
{
}

template <typename T, typename PAllocator>
inline PVectorBase<T, PAllocator>::PVectorBase(const PAllocator& allocator)
	: m_begin(NULL),
	m_end(NULL),
	m_capacityAllocator(NULL, allocator)
{
}

template <typename T, typename PAllocator>
inline PVectorBase<T, PAllocator>::PVectorBase(size_t numObjectsToAllocate, const PAllocator& allocator)
	: m_capacityAllocator(allocator)
{
	m_begin = Allocate(numObjectsToAllocate);
	m_end = m_begin;
	GetSetableInternalAllocator() = m_begin + numObjectsToAllocate;
}

template <typename T, typename PAllocator>
inline PVectorBase<T, PAllocator>::~PVectorBase()
{
	if (m_begin)
	{
		//Need to free the memory safely
		GetInternalAllocator().Free(m_begin, (GetInternalCapacityPtr() - m_begin) * sizeof(T));
	}
}

template <typename T, typename PAllocator>
inline const PAllocator&	PVectorBase<T, PAllocator>::GetAllocator() const noexcept
{
	return GetInternalAllocator();
}


template <typename T, typename PAllocator>
inline PAllocator&			PVectorBase<T, PAllocator>::GetSetableAllocator() noexcept
{
	return GetSetableInternalAllocator();
}

template <typename T, typename PAllocator>
inline void					PVectorBase<T, PAllocator>::SetAllocator(const PAllocator& allocator)
{
	GetSetableInternalAllocator() = allocator;
}

template <typename T, typename PAllocator>
inline T*					PVectorBase<T, PAllocator>::Allocate(size_t numObjectsToAllocate)
{
	// If numObjectsToAllocate is zero, then we allocate no memory and just return nullptr. 
	// This is fine, as our default ctor initializes with NULL pointers. 
	if (numObjectsToAllocate != 0)
	{
		//Allocate memory here and return the pointer of the objects we just allocated
		T* pointer = GetInternalAllocator().Allocate(numObjectsToAllocate * sizeof(T));
		ASSERT_OR_DIE(pointer == nullptr, "The allocation for PVectorBase returned nullptr");

		return pointer;
	}
	else
	{
		return nullptr;
	}
}


template <typename T, typename PAllocator>
inline void					PVectorBase<T, PAllocator>::Free(T* p, size_t numObjectsToFree)
{
	if (p)
	{
		//TODO("Unfuck freeing memory using interal allocator stored");
		GetInternalAllocator().Free(p, numObjectsToFree * sizeof(T));
	}
}


template <typename T, typename PAllocator>
inline size_t				PVectorBase<T, PAllocator>::GetNewCapacity(size_t currentCapacity)
{
	// This needs to return a value of at least currentCapacity and at least 1.
	return (currentCapacity > 0) ? (2 * currentCapacity) : 1;
}
