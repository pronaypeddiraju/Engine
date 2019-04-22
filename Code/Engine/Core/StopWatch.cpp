//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Core/StopWatch.hpp"
#include "Engine/Core/Time.hpp"

//------------------------------------------------------------------------------------------------------------------------------
StopWatch::StopWatch(Clock* clock)
{
	if (clock != nullptr)
	{
		m_clock = clock;
	}
	else
	{
		m_clock = &Clock::m_masterClock;
	}
	
}

//------------------------------------------------------------------------------------------------------------------------------
StopWatch::~StopWatch()
{

}

//------------------------------------------------------------------------------------------------------------------------------
void StopWatch::Start(float time)
{
	m_startTime = (float)GetCurrentTimeSeconds();
	m_duration = time;
}

//------------------------------------------------------------------------------------------------------------------------------
void StopWatch::Set(float time)
{
	m_duration = time;
}

//------------------------------------------------------------------------------------------------------------------------------
void StopWatch::Reset()
{
	m_elapsedTime = 0.f;
}

//------------------------------------------------------------------------------------------------------------------------------
float StopWatch::GetDuration() const
{
	return m_duration;
}

//------------------------------------------------------------------------------------------------------------------------------
float StopWatch::GetElapsedTime() const
{
	return m_elapsedTime;
}

//------------------------------------------------------------------------------------------------------------------------------
float StopWatch::GetRemainingTime() const
{
	return m_duration - m_elapsedTime;
}

//------------------------------------------------------------------------------------------------------------------------------
float StopWatch::GetNormalizedElapsedTime() const
{
	return (m_elapsedTime) / m_duration;
}

//------------------------------------------------------------------------------------------------------------------------------
bool StopWatch::HasElapsed() const
{
	float currentTime = GetCurrentTimeSeconds();
	float nextLap = m_startTime + m_duration;

	return (currentTime >= nextLap);
}

//------------------------------------------------------------------------------------------------------------------------------
uint StopWatch::GetElapseCount() const
{
	float currentTime = GetCurrentTimeSeconds();
	float elapsedTime = currentTime - m_startTime;

	return (uint)(elapsedTime / m_duration);
}

//------------------------------------------------------------------------------------------------------------------------------
bool StopWatch::Decrement()
{
	float currentTime = GetCurrentTimeSeconds(); 
	float nextLap = m_startTime + m_duration;

	if (currentTime >= nextLap) 
	{
		m_elapsedCount++;
		m_startTime += m_duration;
		return true;
	}
	else
	{
		return false;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
uint StopWatch::DecrementAll()
{
	uint count = 0; 
	while (Decrement()) {
		++count; 
	}

	return count; 
}


