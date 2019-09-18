#pragma once

typedef void* HANDLE;
typedef unsigned int uint;

class Semaphore
{
	HANDLE		m_semaphore;

public:
	Semaphore();
	explicit Semaphore(uint initialCount, uint maxCount);
	~Semaphore();

	void		Create(uint initialCount, uint maxCount);
	void		Destroy();
	void		Acquire();
	bool		TryAcquire();
	void		Release(uint count = 1);

	// to make this work like a normal scope lock; 
	inline void Lock()					{ Acquire(); }
	inline bool TryLock()				{ return TryAcquire(); };
	inline void Unlock()				{ Release(1); }
};
