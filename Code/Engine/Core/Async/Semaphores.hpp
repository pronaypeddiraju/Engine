#pragma once

typedef void* HANDLE;
typedef unsigned int uint;

class Semaphore
{
	HANDLE m_semaphore; // HANDLE is a void*, if you want to use that in your header; 
	
	void Create(uint initialCount, uint maxCount);
	void Destroy();
	void Acquire();
	bool TryAcquire();
	void Release(uint count = 1);

	// to make this work like a normal scope lock; 
	inline void lock() { Acquire(); }
	inline bool try_lock() { return TryAcquire(); };
	inline void unlock() { Release(1); }
};
