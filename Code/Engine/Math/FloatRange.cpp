//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Commons/EngineCommon.hpp"

const STATIC FloatRange FloatRange::ZeroToOne(0.f,1.f);

//------------------------------------------------------------------------------------------------------------------------------
void FloatRange::SetFromText( const char* asText )
{
	std::vector<std::string> splitStrings = SplitStringOnDelimiter(asText, '~');
	if(splitStrings.size() == 0 || splitStrings.size() > 2)
	{
		ERROR_AND_DIE(" ERROR: Data from FloatRange SetFromText did not recieve required number of components")
	}
	else
	{
		if(splitStrings.size() == 1)
		{
			minFloat = static_cast<float>(atof(splitStrings[0].c_str()));
			maxFloat = static_cast<float>(atof(splitStrings[0].c_str()));
		}
		else
		{
			minFloat = static_cast<float>(atof(splitStrings[0].c_str()));
			maxFloat = static_cast<float>(atof(splitStrings[1].c_str()));
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------
FloatRange::FloatRange( const FloatRange& copyFrom )
{
	minFloat = copyFrom.minFloat;
	maxFloat = copyFrom.maxFloat;
}

//------------------------------------------------------------------------------------------------------------------------------
FloatRange::FloatRange( float min, float max )
{
	minFloat = min;
	maxFloat = max;
}

//------------------------------------------------------------------------------------------------------------------------------
FloatRange::FloatRange( const char* asText )
{
	SetFromText(asText);
}

