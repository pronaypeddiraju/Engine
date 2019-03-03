//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Math/Vec4.hpp"
#include <vector>
#include <string>

//------------------------------------------------------------------------------------------------------------------------------
const STATIC Vec4 Vec4::ZERO(0.f, 0.f, 0.f, 0.f);
const STATIC Vec4 Vec4::ONE(1.f, 1.f, 1.f, 1.f);

//-----------------------------------------------------------------------------------------------
Vec4::Vec4( const Vec4& copy )
	: x( copy.x )
	, y( copy.y )
	, z( copy.z )
	, w( copy.w )
{
}

//------------------------------------------------------------------------------------------------------------------------------
Vec4::Vec4( float initialX, float initialY, float initialZ, float initialW )
	: x( initialX )
	, y( initialY )
	, z( initialZ )
	, w( initialW )
{
}

//------------------------------------------------------------------------------------------------------------------------------
Vec4::Vec4( const char* asText )
{
	SetFromText(asText);
}

//------------------------------------------------------------------------------------------------------------------------------
void Vec4::SetFromText( const char* asText )
{
	//Read the data, break using the delimiter and save each block to it's respective Vec2 component
	std::vector< std::string> splitStrings = SplitStringOnDelimiter(asText, ',');
	if(splitStrings.size() != 4)
	{
		ERROR_AND_DIE("ERROR: Data from Vec4 SetFromText did not recieve 4 string components");
	}
	else
	{
		x = static_cast<float>(atof(splitStrings[0].c_str()));
		y = static_cast<float>(atof(splitStrings[1].c_str()));
		z = static_cast<float>(atof(splitStrings[2].c_str()));
		w = static_cast<float>(atof(splitStrings[3].c_str()));
	}
}

//------------------------------------------------------------------------------------------------------------------------------
const Vec4 Vec4::operator + ( const Vec4& vecToAdd ) const
{
	return Vec4( x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z , w + vecToAdd.w); 
}

//------------------------------------------------------------------------------------------------------------------------------
const Vec4 Vec4::operator-( const Vec4& vecToSubtract ) const
{
	return Vec4( x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z, w - vecToSubtract.w ); 
}

//------------------------------------------------------------------------------------------------------------------------------
const Vec4 Vec4::operator*( float uniformScale ) const
{
	return Vec4( x * uniformScale, y * uniformScale, z * uniformScale, w * uniformScale ); 
}

//------------------------------------------------------------------------------------------------------------------------------
const Vec4 Vec4::operator/( float inverseScale ) const
{
	return Vec4( x / inverseScale, y / inverseScale, z / inverseScale, w / inverseScale ); 
}

//------------------------------------------------------------------------------------------------------------------------------
void Vec4::operator+=( const Vec4& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y; 
	z += vecToAdd.z;
	w += vecToAdd.w;
}

//------------------------------------------------------------------------------------------------------------------------------
void Vec4::operator-=( const Vec4& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
	w -= vecToSubtract.w;
}

//------------------------------------------------------------------------------------------------------------------------------
void Vec4::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
	w *= uniformScale;
}

//------------------------------------------------------------------------------------------------------------------------------
void Vec4::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor;
	y /= uniformDivisor;
	z /= uniformDivisor;
	w /= uniformDivisor;
}

//------------------------------------------------------------------------------------------------------------------------------
void Vec4::operator=( const Vec4& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
	w = copyFrom.w;
}

//------------------------------------------------------------------------------------------------------------------------------
const Vec4 operator*( float uniformScale, const Vec4& vecToScale )
{
	return Vec4( vecToScale.x * uniformScale, vecToScale.y * uniformScale, vecToScale.z * uniformScale, vecToScale.w * uniformScale);
}

//------------------------------------------------------------------------------------------------------------------------------
bool Vec4::operator==( const Vec4& compare ) const
{
	if(x == compare.x && y == compare.y && z == compare.z && w == compare.w)
		return true;
	else
		return false;
}

//------------------------------------------------------------------------------------------------------------------------------
bool Vec4::operator!=( const Vec4& compare ) const
{
	if(x == compare.x && y == compare.y && z == compare.z && w == compare.w)
		return false;
	else
		return true;
}
