//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Commons/ErrorWarningAssert.hpp"
#include "Engine/Core/Time.hpp"

// Helpful Function to make values more human readable; 
// if time is < 1ms, display in micro seconds, ex 0.0000124283 -> "12.428us"
// if time is < 1s, display in ms (with 3 values after decimal), ex: 0.23434315 -> "234.343ms"
// otherwise, display in seconds (with 3 values after decimal) -> ex 120482.294719 -> "120482.295 s"
std::string GetProfileTimeString(double seconds);

//------------------------------------------------------------------------------------------------------------------------------
class LogProfileScope
{
public:
	LogProfileScope(char const *name)
	{
		m_name = name;
		m_start_time = GetCurrentTimeHPC();
	}

	~LogProfileScope()
	{
		uint64_t duration = GetCurrentTimeHPC() - m_start_time;
		DebuggerPrintf("%s took %s (%llu HPC Units) \n", m_name, GetProfileTimeString((double)duration).c_str(), (double)duration);
	}

	uint64_t m_start_time;
	char const* m_name;
};

#define PROFILE_LOG_SCOPE( tag )  LogProfileScope _____scopeLog ## __LINE__ ## ( tag )
