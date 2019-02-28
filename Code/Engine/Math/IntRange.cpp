//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Math/IntRange.hpp"

const STATIC IntRange IntRange::ZeroToOne(0,1);
const STATIC IntRange IntRange::Zero(0,0);
const STATIC IntRange IntRange::One(1,1);

//------------------------------------------------------------------------------------------------------------------------------
void IntRange::SetFromText( const char* asText )
{
	std::vector<std::string> splitStrings = SplitStringOnDelimiter(asText, '~');
	if(splitStrings.size() == 0 || splitStrings.size() > 2)
	{
		ERROR_AND_DIE(" ERROR: Data from IntRange SetFromText did not recieve required number of components")
	}
	else
	{
		if(splitStrings.size() == 1)
		{
			minInt = atoi(splitStrings[0].c_str());
			maxInt = atoi(splitStrings[0].c_str());
		}
		else
		{
			minInt = atoi(splitStrings[0].c_str());
			maxInt = atoi(splitStrings[1].c_str());
		}
	}
}

IntRange::IntRange( const IntRange& copyFrom )
{
	minInt = copyFrom.minInt;
	maxInt = copyFrom.maxInt;
}

IntRange::IntRange( int min, int max )
{
	minInt = min;
	maxInt = max;
}

IntRange::IntRange( const char* asText )
{
	SetFromText(asText);
}

