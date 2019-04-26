//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Core/StopWatch.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Math/MathUtils.hpp"

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
	m_elapsedTime = 0.f;
	m_elapsedCount = 0.f;
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
	m_elapsedCount = 0;
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
float StopWatch::GetNormalizedElapsedTime()
{
	float currentTime = (float)GetCurrentTimeSeconds();
	m_elapsedTime = currentTime - m_startTime;

	float mapped = RangeMapFloat(m_elapsedTime, 0.f, m_duration, 0.f, 1.f);

	return mapped;
}

//------------------------------------------------------------------------------------------------------------------------------
bool StopWatch::HasElapsed()
{
	float currentTime = (float)GetCurrentTimeSeconds();
	float nextLap = m_startTime + m_duration;

	return (currentTime >= nextLap);
}

//------------------------------------------------------------------------------------------------------------------------------
uint StopWatch::GetElapseCount()
{
	float currentTime = (float)GetCurrentTimeSeconds();
	m_elapsedTime = currentTime - m_startTime;

	return (uint)(m_elapsedTime / m_duration);
}

//------------------------------------------------------------------------------------------------------------------------------
bool StopWatch::Decrement()
{
	float currentTime = (float)GetCurrentTimeSeconds(); 
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


