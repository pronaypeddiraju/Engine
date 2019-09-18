#pragma once
#include "Engine/Commons/Callstack.hpp"
#include <thread>
#include "Engine/Core/Async/MPSCAsyncRingBuffer.hpp"
#include "Engine/Core/Async/Semaphores.hpp"


//------------------------------------------------------------------------------------------------------------------------------
struct LogObject_T
{
	uint64_t			hpcTime;
	std::string			filter;
	std::string			line;
	Callstack			callstack;
};

//------------------------------------------------------------------------------------------------------------------------------
class LogSystem
{
public:
	LogSystem(const char* fileName);
	~LogSystem();

	std::thread			m_thread;
	std::string			m_filename;

	MPSCRingBuffer		m_messages;
	Semaphore			m_semaphore;

	std::ofstream*		m_fileStream = nullptr;

	bool				m_isRunning = true;

	void				LogSystemInit();
	void				LogSystemShutDown();

	void				Logf(char const* filter, char const* format, ...);

	void				WaitForWork() { m_semaphore.Acquire(); }
	void				SignalWork()			{ m_semaphore.Release(1); }

	bool				IsRunning() const		{ return m_isRunning; }
	void				Stop()					{ m_isRunning = false; }
	void				SetDestroy()			{ m_canDestroy = true; }
private:
	bool				m_canDestroy = false;
};