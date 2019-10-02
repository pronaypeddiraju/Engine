#pragma once

//------------------------------------------------------------------------------------------------------------------------------
class InternalAllocator  
{
public:
	
	virtual void*	Allocate(size_t size) = 0;
	virtual void	Free(void* ptr) = 0;

	template <typename T, typename ...ARGS>
	T* Create(ARGS&& ...args)
	{
		void* mem = this->Allocate(sizeof(T));
		if (mem != nullptr) 
		{
			// new in place
			//We want to perfect forward our arguments
			new (mem) T(std::forward<ARGS>(args)...);
		}

		return (T*)mem;
	}

	template <typename T> 
	void Destroy(T* obj)
	{
		if (obj != nullptr) 
		{
			obj->~T();
			this->Free(obj);
		}
	}
};

