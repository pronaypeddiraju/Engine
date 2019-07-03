#include "Engine/Core/Profiler.hpp"

//------------------------------------------------------------------------------------------------------------------------------
std::string GetProfileTimeString(double seconds)
{
	double timeSeconds = GetHPCToSeconds((uint64_t)seconds);
	std::string timeString;

	if (timeSeconds <= 0.000001)
	{
		double timeMicroseconds = timeSeconds / 1000000;
		timeString = std::to_string(timeMicroseconds) + "microseconds";
	}
	else if (timeSeconds <= 0.001)
	{
		double timeMilliseconds = timeSeconds / 1000;
		timeString = std::to_string(timeMilliseconds) + "milliseconds";
	}
	else
	{
		timeString = std::to_string(timeSeconds) + "seconds";
	}

	return timeString;
}

