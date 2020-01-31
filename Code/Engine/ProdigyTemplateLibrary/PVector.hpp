#pragma once
//------------------------------------------------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////

//This class is the implementation of a dynamic array similar to that of std::vector<T>
//The intent of this class is to keep a dynamic array and use PVectorBase
//to allocate and free memory and not have to worry about that in this class implementation
//This class is simply handling the dyamic array creation, usage and deletion

//////////////////////////////////////////////////////////////////////////

#include "Engine/ProdigyTemplateLibrary/PVectorBase.hpp"

//------------------------------------------------------------------------------------------------------------------------------
template <typename T, typename PAllocator = UntrackedAllocator>
class PVector : public PVectorBase<T, PAllocator>
{
public:
	
	typedef T*				iterator;      
	typedef const T*        const_iterator;

public:

	PVector() noexcept;
	explicit PVector(const PAllocator& allocator) noexcept;
	explicit PVector(size_t size, const PAllocator& allocator = UntrackedAllocator);
	PVector(size_t size, const T& value, const PAllocator& allcator = UntrackedAllocator);
	PVector(const T& x);
	PVector(const T& x, const PAllocator& allocator);
	PVector(T& x) noexcept;
	PVector(T& x, const PAllocator& allcator);
	PVector(std::initializer_list<T> ilist, const PAllocator& allocator = UntrackedAllocator);

	template <typename InputIterator>
	PVector(InputIterator first, InputIterator last, const PAllocator& allocator = UntrackedAllocator);

	~PVector();

	//Comparison Operators
	PVector<T, PAllocator>& operator=(const PVector<T, PAllocator>& x);
	PVector<T, PAllocator>& operator=(std::initializer_list<value_type> ilist);
	PVector<T, PAllocator>& operator=(PVector<T, PAllocator>&& x); 

	//Internal functions
	
	void Swap(PVector<T, PAllocator>& x);

	void Assign(size_t size, const T& value);

	template <typename InputIterator>
	void Assign(InputIterator first, InputIterator last);

	void Assign(std::initializer_list<value_type> ilist);

	iterator		Begin()		noexcept;

	iterator		Begin()		noexcept;
	const_iterator	Begin()		const noexcept;
	const_iterator	CBegin()	const noexcept;

	iterator		End()		noexcept;
	const_iterator	End()		const noexcept;
	const_iterator	CEnd()		const noexcept;

	bool			Empty()		const noexcept;
	size_t			Size()		const noexcept;
	size_t			Capacity()	const noexcept;

	void			Resize(size_t n, const T& value);
	void			Resize(size_t n);
	void			Reserve(size_t n);
	void			SetCapacity(size_t n = PVectorBase<T, PAllocator>::nonValidPos);   // Revises the capacity to the user-specified value. Resizes the container to match the capacity if the requested capacity n is less than the current size. If n == nonValidPos then the capacity is reallocated (if necessary) such that capacity == size.
	void			ShrinkToFit();                          //Basically the same as SetCapacity but intuitive to have

	T*				Data() noexcept;
	const T* 		Data() const noexcept;

	T&				operator[](size_t n);
	const T&		operator[](size_t n) const;

	T&				At(size_type n);
	const T&		At(size_type n) const;

	T&				Front();
	const T&		Front() const;

	T&				Back();
	const T&		Back() const;

	void			PushBack(const T& value);
	T&				PushBack();
	void*			PushBackUninitialized();
	void			PushBack(T&& value);
	void			PopBack();

	template<class... Args>
	iterator		Emplace(const_iterator position, Args&&... args);

	template<class... Args>
	T&				EmplaceBack(Args&&... args);

	iterator		Insert(const_iterator position, const T& value);
	iterator		Insert(const_iterator position, size_t n, const T& value);
	iterator		Insert(const_iterator position, T&& value);
	iterator		Insert(const_iterator position, std::initializer_list<T> ilist);

	template <typename InputIterator>
	iterator		Insert(const_iterator position, InputIterator first, InputIterator last);

	iterator		EraseFirst(const T& value);
	iterator		EraseFirstUnsorted(const T& value);
	
	iterator		Erase(const_iterator position);
	iterator		Erase(const_iterator first, const_iterator last);
	iterator		EraseUnsorted(const_iterator position);   //Overwrites the hash table just like erase does, just doesn't preserve order. faster because it copies the last item in the vector over erased position.

	void			Clear() noexcept;
	void			ResetKeepMemeory() noexcept;  // This is for speed. don't call destructors, don't deallocate, simply re-assign to a state of emptiness.

	bool			Validate() const noexcept;
	int				ValidateIterator(const_iterator i) const noexcept;

};

//------------------------------------------------------------------------------------------------------------------------------
template <typename T, typename PAllocator /*= UntrackedAllocator*/>
PVector<T, PAllocator>::PVector() noexcept
{
	//Empty constructor
}

//------------------------------------------------------------------------------------------------------------------------------
template <typename T, typename PAllocator /*= UntrackedAllocator*/>
PVector<T, PAllocator>::PVector(const PAllocator& allocator) noexcept
{
	//Empty constructor
}

template <typename T, typename PAllocator /*= UntrackedAllocator*/>
PVector<T, PAllocator>::PVector(size_t size, const PAllocator& allocator /*= UntrackedAllocator*/)
	:	PVectorBase<T, PAllocator>(size, allocator)
{

}
