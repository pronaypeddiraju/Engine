#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Commons/EngineCommon.hpp"
#include <vector>
#include <string>

//Debug only
#include <stdio.h>

//-----------------------------------------------------------------------------------------------
Vec3::Vec3( const Vec3& copy )
	: x( copy.x )
	, y( copy.y )
	, z( copy.z )
{
}

Vec3::Vec3( float initialX, float initialY, float initialZ )
	: x( initialX )
	, y( initialY )
	, z( initialZ )
{
}

Vec3::Vec3( const char* asText )
{
	SetFromText(asText);
}

//------------------------------------------------------------------------------------------------------------------------------

//Get length of vector
float Vec3::GetLength() const
{
	return sqrtf(x*x + y*y + z*z);
}

//Get length using only X and Y coordinated of the vector
float Vec3::GetLengthXY() const
{
	return sqrtf(x*x + y*y);
}

//Return squared length of the vector
float Vec3::GetLengthSquared() const
{
	return (x*x + y*y + z*z);
}

//Return squared length of XY magnitude
float Vec3::GetLengthSquaredXY() const
{
	return (x*x + y*y);
}

//Rotate the vector about an angle on Z
float Vec3::GetAngleAboutZDegrees() const
{
	return ATan2Degrees(y ,x);
}

//Rotate the vector about an angle on Z
float Vec3::GetAngleAboutZRadians() const
{
	return atan2f(y, x);
}

//Rotated vector about Z degrees
const Vec3 Vec3::GetRotatedAboutZDegrees( float degreesToRotateAroundZ ) const
{
	float radiansToRotate = DegreesToRadians(degreesToRotateAroundZ);
	Vec3 resultVec = GetRotatedAboutZRadians(radiansToRotate);	
	return resultVec;
}

//Rotated vector about Z radians
const Vec3 Vec3::GetRotatedAboutZRadians( float radiansToRotateAroundZ ) const
{
	//printf("\n Radians to Rotate around : %f", radiansToRotateAroundZ);

	float r = GetLengthXY();
	//printf("\n Length: %f ", r);

	float currentRadians = GetAngleAboutZRadians();
	//printf("\n Current Radians: %f", currentRadians);

	float newRadians = currentRadians + radiansToRotateAroundZ;
	//printf("\n New Radians: %f", newRadians);

	float newX = r * cosf(newRadians);
	float newY = r * sinf(newRadians);

	//printf("\n New X: %f", newX);
	//printf("\n New Y: %f", newY);
	//printf("\n New Z: %f", z);
	return Vec3(newX, newY, z);
}

void Vec3::SetFromText( const char* asText )
{
	//Read the data, break using the delimiter and save each block to it's respective Vec2 component
	std::vector< std::string> splitStrings = SplitStringOnDelimiter(asText, ',');
	if(splitStrings.size() != 3)
	{
		ERROR_AND_DIE("ERROR: Data from Vec3 SetFromText did not recieve 3 string components");
	}
	else
	{
		x = static_cast<float>(atof(splitStrings[0].c_str()));
		y = static_cast<float>(atof(splitStrings[1].c_str()));
		z = static_cast<float>(atof(splitStrings[2].c_str()));
	}
}

void Vec3::ClampLengthXY( float maxLength )
{
	float r = sqrtf(x*x + y*y);
	if(r< maxLength)
		return;
	else
	{
		r = maxLength;
		float vectorAngle = atan2f(y, x);
		x = r * cosf(vectorAngle);
		y = r * sinf(vectorAngle);
	}
}

//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator + ( const Vec3& vecToAdd ) const
{
	return Vec3( x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z ); 
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator-( const Vec3& vecToSubtract ) const
{
	return Vec3( x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z ); 
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator*( float uniformScale ) const
{
	return Vec3( x * uniformScale, y * uniformScale, z * uniformScale ); 
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator/( float inverseScale ) const
{
	return Vec3( x / inverseScale, y / inverseScale, z / inverseScale ); 
}


void Vec3::SetLengthXY( float setLength )
{
	float newLength = setLength;
	float vectorAngle = atan2f(y, x);
	x = newLength * cosf(vectorAngle);
	y = newLength * sinf(vectorAngle);
	z = 0.f;
}

//-----------------------------------------------------------------------------------------------
void Vec3::operator+=( const Vec3& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y; 
	z += vecToAdd.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator-=( const Vec3& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor;
	y /= uniformDivisor;
	z /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator=( const Vec3& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}


//-----------------------------------------------------------------------------------------------
const Vec3 operator*( float uniformScale, const Vec3& vecToScale )
{
	return Vec3( vecToScale.x * uniformScale, vecToScale.y * uniformScale, vecToScale.z * uniformScale);
}


//-----------------------------------------------------------------------------------------------
bool Vec3::operator==( const Vec3& compare ) const
{
	if(x == compare.x && y == compare.y && z == compare.z)
		return true;
	else
		return false;
}


//-----------------------------------------------------------------------------------------------
bool Vec3::operator!=( const Vec3& compare ) const
{
	if(x == compare.x && y == compare.y && z == compare.z)
		return false;
	else
		return true;
}

