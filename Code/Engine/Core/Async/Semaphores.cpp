#include "Engine/Core/Async/Semaphores.hpp"

#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>

//------------------------------------------------------------------------------------------------------------------------------
Semaphore::Semaphore(uint initialCount, uint maxCount)
{
	Create(initialCount, maxCount);
}

//------------------------------------------------------------------------------------------------------------------------------
Semaphore::Semaphore()
{
	//Does nothing, user must call Create before use
}

//------------------------------------------------------------------------------------------------------------------------------
Semaphore::~Semaphore()
{
	Destroy();
}

void Semaphore::Create(uint initialCount, uint maxCount)
{
	// Creating/Initializing
	m_semaphore = ::CreateSemaphore(nullptr,  // security attributes - ignore 
		initialCount,                      // count this starts at
		maxCount,                          // max count this semaphore can reach
		nullptr);                          // name, if used across processes
}

// note: should probably call from deconstructor
void Semaphore::Destroy()
{
	if (m_semaphore != nullptr) {
		::CloseHandle(m_semaphore);
		m_semaphore = nullptr;
	}
}

// Acquire a Seamphore
// this will block until the semaphore becomes invalid (destroyed)
// or succeeds
void Semaphore::Acquire()
{
	::WaitForSingleObject(m_semaphore, // object to wait on
		INFINITE);                      // time to wait in milliseconds
}

// may or may not succeed
// if returns true, the counter was decremented
// if returns false, the counter was 0 and unable to be decremented
bool Semaphore::TryAcquire()
{
	DWORD result = ::WaitForSingleObject(m_semaphore, 0);
	return (result == WAIT_OBJECT_0); // we successfully waited on the first object (m_semaphroe)
}

// releases teh seamphore - ie, adds to the counter up to max
void Semaphore::Release(uint count)
{
	::ReleaseSemaphore(m_semaphore,
		count,      // how many to add to the semaphore
		nullptr);  // out for previous count
}