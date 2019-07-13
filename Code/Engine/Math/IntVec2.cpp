//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include <vector>
#include <string>

const STATIC IntVec2 IntVec2::ZERO(0, 0);
const STATIC IntVec2 IntVec2::ONE(1, 1);

//-----------------------------------------------------------------------------------------------
IntVec2::IntVec2( const IntVec2& copy )
	: x( copy.x )
	, y( copy.y )
{
}


//-----------------------------------------------------------------------------------------------
IntVec2::IntVec2( int initialX, int initialY )
	: x( initialX )
	, y( initialY )
{
}

//------------------------------------------------------------------------------------------------------------------------------
IntVec2::IntVec2()
{

}

//------------------------------------------------------------------------------------------------------------------------------
IntVec2::IntVec2( const char* asText )
{
	SetFromText(asText);
}

//------------------------------------------------------------------------------------------------------------------------------
IntVec2::IntVec2(const Vec2& vec2)
{
	x = (int)vec2.x;
	y = (int)vec2.y;
}

//------------------------------------------------------------------------------------------------------------------------------
IntVec2::~IntVec2()
{

}

//------------------------------------------------------------------------------------------------------------------------------
void IntVec2::SetIntVec2( IntVec2 inValue )
{
	x = inValue.x;
	y = inValue.y;
}

//------------------------------------------------------------------------------------------------------------------------------
void IntVec2::SetFromText( const char* asText )
{
	//Break using delimiter
	std::vector< std::string> splitStrings = SplitStringOnDelimiter(asText, ',');
	if(splitStrings.size() != 2)
	{
		ERROR_AND_DIE("ERROR: Data from IntVec2 SetFromText did not recieve 2 string components");
	}
	else
	{
		x = atoi(splitStrings[0].c_str());
		y = atoi(splitStrings[1].c_str());
	}
}

//------------------------------------------------------------------------------------------------------------------------------
IntVec2 IntVec2::GetIntVec2() const
{
	IntVec2 currectVec;
	currectVec.x = x;
	currectVec.y = y;
	return currectVec;
}

//------------------------------------------------------------------------------------------------------------------------------
std::string IntVec2::GetAsString() const
{
	std::string string;
	string = std::to_string(x);
	string += ",";
	string += std::to_string(y);

	return string;
}

//------------------------------------------------------------------------------------------------------------------------------
bool IntVec2::IsInBounds(const IntVec2& bounds) const
{
	if (x >= 0 && x < bounds.x && y >= 0 && y < bounds.y)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//-----------------------------------------------------------------------------------------------
const IntVec2 IntVec2::operator + ( const IntVec2& vecToAdd ) const
{
	return IntVec2( x + vecToAdd.x, y + vecToAdd.y ); 
}

//-----------------------------------------------------------------------------------------------
const IntVec2 IntVec2::operator-( const IntVec2& vecToSubtract ) const
{
	return IntVec2( x - vecToSubtract.x, y - vecToSubtract.y ); 
}

//------------------------------------------------------------------------------------------------------------------------------
bool IntVec2::operator<(const IntVec2& compare) const
{
	if (x < compare.x && y < compare.y)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//-----------------------------------------------------------------------------------------------
const IntVec2 IntVec2::operator*( int uniformScale ) const
{
	return IntVec2( x * uniformScale, y * uniformScale ); 
}

//-----------------------------------------------------------------------------------------------
const IntVec2 IntVec2::operator/( int inverseScale ) const
{
	return IntVec2( x / inverseScale, y / inverseScale ); 
}

//-----------------------------------------------------------------------------------------------
void IntVec2::operator+=( const IntVec2& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y; 
}

//-----------------------------------------------------------------------------------------------
void IntVec2::operator-=( const IntVec2& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}

//-----------------------------------------------------------------------------------------------
void IntVec2::operator*=( const int uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
}

//-----------------------------------------------------------------------------------------------
void IntVec2::operator/=( const int uniformDivisor )
{
	x /= uniformDivisor;
	y /= uniformDivisor;
}

//-----------------------------------------------------------------------------------------------
void IntVec2::operator=( const IntVec2& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}

//-----------------------------------------------------------------------------------------------
const IntVec2 operator*( int uniformScale, const IntVec2& vecToScale )
{
	return IntVec2( vecToScale.x * uniformScale, vecToScale.y * uniformScale); 
}

//-----------------------------------------------------------------------------------------------
bool IntVec2::operator==( const IntVec2& compare ) const
{
	if(x == compare.x && y == compare.y)
		return true;
	else
		return false;
}

//-----------------------------------------------------------------------------------------------
bool IntVec2::operator!=( const IntVec2& compare ) const
{
	if(x == compare.x && y == compare.y)
		return false;
	else
		return true;
}