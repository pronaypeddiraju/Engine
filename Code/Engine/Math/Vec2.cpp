#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Commons/EngineCommon.hpp"
#include <vector>
#include <string>

const STATIC Vec2 Vec2::ZERO(0.f,0.f);
const STATIC Vec2 Vec2::ONE(1.f, 1.f);

const STATIC Vec2 Vec2::ALIGN_CENTERED(0.5f, 0.5f);

const STATIC Vec2 Vec2::ALIGN_LEFT_BOTTOM(0.0f, 0.0f);
const STATIC Vec2 Vec2::ALIGN_LEFT_CENTERED(0.0f, 0.5f);
const STATIC Vec2 Vec2::ALIGN_LEFT_TOP(0.0f, 1.0f);

const STATIC Vec2 Vec2::ALIGN_RIGHT_BOTTOM(1.0f, 0.0f);
const STATIC Vec2 Vec2::ALIGN_RIGHT_CENTERED(1.f, 0.5f);
const STATIC Vec2 Vec2::ALIGN_RIGHT_TOP(1.f, 1.f);

const STATIC Vec2 Vec2::ALIGN_TOP_CENTERED(0.5f, 1.f);
const STATIC Vec2 Vec2::ALIGN_BOTTOM_CENTERED(0.5f, 0.f);

//-----------------------------------------------------------------------------------------------
Vec2::Vec2( const Vec2& copy )
   	: x( copy.x )
	, y( copy.y )
{
}


//-----------------------------------------------------------------------------------------------
Vec2::Vec2( float initialX, float initialY )
	: x( initialX )
	, y( initialY )
{
}

Vec2::Vec2( const char* asText )
{
	SetFromText(asText);
}

//Returns the length of the vector
float Vec2::GetLength() const
{
	return sqrtf( x*x + y*y );
}

//Returns the length of a vector squared
float Vec2::GetLengthSquared() const
{
	return (x*x + y*y);
}

//Get the angle in degrees for a vector
float Vec2::GetAngleDegrees() const
{
	return ATan2Degrees(y ,x);
}

//Get the angle in radians for a vector
float Vec2::GetAngleRadians() const
{
	return atan2f(y, x);
}

//Rotate vector by 90 degrees
const Vec2 Vec2::GetRotated90Degrees() const
{
	return Vec2(-y, x);
}

//Rotate vector by -90 degrees
const Vec2 Vec2::GetRotatedMinus90Degrees() const
{
	return Vec2(y, -x);
}

//Rotate vector by degrees given to you
const Vec2 Vec2::GetRotatedDegrees( float degreesToRotate ) const
{
	float radiansToRotate = DegreesToRadians(degreesToRotate);
	Vec2 resultVec = GetRotatedRadians(radiansToRotate);	
	return resultVec;
}

//Rotate vector by radians
const Vec2 Vec2::GetRotatedRadians( float radiansToRotate ) const
{
	float r = GetLength();

	float currentRadians = GetAngleRadians();
	float newRadians = currentRadians + radiansToRotate;
	float newX = r * cosf(newRadians);
	float newY = r * sinf(newRadians);

	return Vec2(newX, newY);
}

//Clamps the vector a specific length
const Vec2 Vec2::GetClamped( float maxLength ) const
{
	float r = sqrtf(x*x + y*y);
	if(r > maxLength)
	{
		//need to clamp
		r = maxLength;
		float vectorAngle = atan2f(y, x);
		float xNew = r * cosf(vectorAngle);
		float yNew = r * sinf(vectorAngle);
		return Vec2(xNew, yNew);
	}
	else
	{
		return Vec2(x, y);
	}
}

//Normalizes a vector
const Vec2 Vec2::GetNormalized() const
{
	if(GetLength() == NULL)
	{
		return Vec2(0.0f ,0.0f);
	}

	float xNorm = x / sqrtf(x*x + y*y);
	float yNorm = y / sqrtf(x*x + y*y);
	return Vec2(xNorm, yNorm);
}


//Make a vec2 from polar degrees
const Vec2 Vec2::MakeFromPolarDegrees( const float polarDegrees, float r) 
{
	float degToRad = DegreesToRadians(polarDegrees);
	Vec2 resultVec = MakeFromPolarRadians(degToRad, r);
	return resultVec;
}

//Make vec2 from polar radians
const Vec2 Vec2::MakeFromPolarRadians( const float polarRadians, float r) 
{
	float newX = r * cosf(polarRadians);
	float newY = r * sinf(polarRadians);

	return Vec2(newX, newY);
}

//Clamp length to a maxLength if it's longer
void Vec2::ClampLength( float maxLength )
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

//Set the length of the vector to setLength
void Vec2::SetLength( float setLength )
{
	float newLength = setLength;
	float vectorAngle = atan2f(y, x);
	x = newLength * cosf(vectorAngle);
	y = newLength * sinf(vectorAngle);
}

//Set the vector angle to specified degrees
void Vec2::SetAngleDegrees( float setAngleDeg )
{	
	float angleInRadians = DegreesToRadians(setAngleDeg);
	SetAngleRadians(angleInRadians);
}

//Set the vector angle to specified radians
void Vec2::SetAngleRadians( float setAngleRad )
{
	float r = sqrtf(x*x + y*y);
	x = r * cosf(setAngleRad);
	y = r * sinf(setAngleRad);
}

//Set the polar degrees to vector
void Vec2::SetPolarDegrees( float setPolarDeg, float r )
{	
	Vec2 newVec = GetVectorFromAngle(setPolarDeg);
	newVec.SetLength(r);
	x = newVec.x;
	y = newVec.y;
}


void Vec2::SetPolarRadians( float setPolarRad, float r )
{
	Vec2 newVec = GetVectorFromAngleRadians(setPolarRad);
	newVec.SetLength(r);
	x = newVec.x;
	y = newVec.y;
}

void Vec2::RotateRadians(float radiansToRotate)
{
	float r = GetLength();
	float currentRadians = GetAngleRadians();
	float newRadians = currentRadians + radiansToRotate;
	x = r * cosf(newRadians);
	y = r * sinf(newRadians);
}

void Vec2::RotateDegrees( float degreesToRotate )
{
	float degInRadians = DegreesToRadians(degreesToRotate);
	RotateRadians(degInRadians);
}

void Vec2::Rotate90Degrees()
{
	float tempX = x;
	x = -y;
	y = tempX;
}

void Vec2::RotateMinus90Degrees()
{
	float tempX = x;
	x = y;
	y = -tempX;
}

//Normalize the vector
void Vec2::Normalize()
{	
	if(x == 0 && y == 0)
		return;

	float r = sqrtf(x*x + y*y);
	x /= r;
	y /= r;
}

//Normalize and get old length
float Vec2::NormalizeAndGetOldLength()
{
	float vecLength = GetLength();
	Normalize();
	return vecLength;
}

void Vec2::SetFromText( const char* asText )
{
	//Read the data, break using the delimiter and save each block to it's respective Vec2 component
	std::vector< std::string> splitStrings = SplitStringOnDelimiter(asText, ',');
	if(splitStrings.size() != 2)
	{
		ERROR_AND_DIE("ERROR: Data from Vec2 SetFromText did not recieve 2 string components");
	}
	else
	{
		x = static_cast<float>(atof(splitStrings[0].c_str()));
		y = static_cast<float>(atof(splitStrings[1].c_str()));
	}
}

const float Vec2::GetX()
{
	return x;
}

const float Vec2::GetY()
{
	return y;
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator + ( const Vec2& vecToAdd ) const
{
	return Vec2( x + vecToAdd.x, y + vecToAdd.y ); // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator-( const Vec2& vecToSubtract ) const
{
	return Vec2( x - vecToSubtract.x, y - vecToSubtract.y ); // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator*( float uniformScale ) const
{
	return Vec2( x * uniformScale, y * uniformScale ); // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator/( float inverseScale ) const
{
	return Vec2( x / inverseScale, y / inverseScale ); 
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator+=( const Vec2& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y; 
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator-=( const Vec2& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor;
	y /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator=( const Vec2& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}


//-----------------------------------------------------------------------------------------------
const Vec2 operator*( float uniformScale, const Vec2& vecToScale )
{
	return Vec2( vecToScale.x * uniformScale, vecToScale.y * uniformScale); // #MP1Fixme
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator==( const Vec2& compare ) const
{
	if(x == compare.x && y == compare.y)
		return true;
	else
		return false;
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator!=( const Vec2& compare ) const
{
	if(x == compare.x && y == compare.y)
		return false;
	else
		return true;
}

