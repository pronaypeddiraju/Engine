//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Math/Vec3.hpp"
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include <vector>
#include <string>

//------------------------------------------------------------------------------------------------------------------------------
const STATIC Vec3 Vec3::ZERO(0.f, 0.f, 0.f);
const STATIC Vec3 Vec3::ONE(1.f, 1.f, 1.f);
const STATIC Vec3 Vec3::FRONT(0.f, 0.f, 1.f);
const STATIC Vec3 Vec3::FORWARD(0.f, 0.f, 1.f);
const STATIC Vec3 Vec3::BACK(0.f, 0.f, -1.f);
const STATIC Vec3 Vec3::LEFT(-1.f, 0.f, 0.f);
const STATIC Vec3 Vec3::RIGHT(1.f, 0.f, 0.f);
const STATIC Vec3 Vec3::UP(0.f, 1.f, 0.f);
const STATIC Vec3 Vec3::DOWN(0.f, -1.f, 0.f);

//------------------------------------------------------------------------------------------------------------------------------
Vec3::Vec3( const Vec3& copy )
	: x( copy.x )
	, y( copy.y )
	, z( copy.z )
{
}

//------------------------------------------------------------------------------------------------------------------------------
Vec3::Vec3( float initialX, float initialY, float initialZ )
	: x( initialX )
	, y( initialY )
	, z( initialZ )
{
}

//------------------------------------------------------------------------------------------------------------------------------
Vec3::Vec3( const char* asText )
{
	SetFromText(asText);
}

//------------------------------------------------------------------------------------------------------------------------------
Vec3::Vec3( const Vec2& copyFrom )
	: x( copyFrom.x )
	, y( copyFrom.y )
	, z( 0.f )
{
}

//------------------------------------------------------------------------------------------------------------------------------
float Vec3::GetLength() const
{
	return sqrtf(x*x + y*y + z*z);
}

//------------------------------------------------------------------------------------------------------------------------------
float Vec3::GetLengthXY() const
{
	return sqrtf(x*x + y*y);
}

//------------------------------------------------------------------------------------------------------------------------------
float Vec3::GetLengthXZ() const
{
	return sqrtf(x*x + z*z);
}

//------------------------------------------------------------------------------------------------------------------------------
float Vec3::GetLengthYZ() const
{
	return sqrtf(y*y + z*z);
}

//------------------------------------------------------------------------------------------------------------------------------
float Vec3::GetLengthSquared() const
{
	return (x*x + y*y + z*z);
}

//------------------------------------------------------------------------------------------------------------------------------
float Vec3::GetLengthSquaredXY() const
{
	return (x*x + y*y);
}

//------------------------------------------------------------------------------------------------------------------------------
float Vec3::GetAngleAboutZDegrees() const
{
	return ATan2Degrees(y ,x);
}

//------------------------------------------------------------------------------------------------------------------------------
float Vec3::GetAngleAboutZRadians() const
{
	return atan2f(y, x);
}

//------------------------------------------------------------------------------------------------------------------------------
float Vec3::GetAngleAboutYDegrees() const
{
	return ATan2Degrees(z ,x);
}

//------------------------------------------------------------------------------------------------------------------------------
float Vec3::GetAngleAboutYRadians() const
{
	return atan2f(z, x);
}

//------------------------------------------------------------------------------------------------------------------------------
float Vec3::GetAngleAboutXDegrees() const
{
	return ATan2Degrees(z ,y);
}

//------------------------------------------------------------------------------------------------------------------------------
float Vec3::GetAngleAboutXRadians() const
{
	return atan2f(z, y);
}

//------------------------------------------------------------------------------------------------------------------------------
const Vec3 Vec3::GetRotatedAboutZDegrees( float degreesToRotateAroundZ ) const
{
	float radiansToRotate = DegreesToRadians(degreesToRotateAroundZ);
	Vec3 resultVec = GetRotatedAboutZRadians(radiansToRotate);	
	return resultVec;
}

//------------------------------------------------------------------------------------------------------------------------------
const Vec3 Vec3::GetRotatedAboutZRadians( float radiansToRotateAroundZ ) const
{
	float r = GetLengthXY();

	float currentRadians = GetAngleAboutZRadians();

	float newRadians = currentRadians + radiansToRotateAroundZ;

	float newX = r * cosf(newRadians);
	float newY = r * sinf(newRadians);

	return Vec3(newX, newY, z);
}

//------------------------------------------------------------------------------------------------------------------------------
const Vec3 Vec3::GetRotatedAboutYDegrees( float degreesToRotateAroundY ) const
{
	float radiansToRotate = DegreesToRadians(degreesToRotateAroundY);
	Vec3 resultVec = GetRotatedAboutYRadians(radiansToRotate);	
	return resultVec;
}

//------------------------------------------------------------------------------------------------------------------------------
const Vec3 Vec3::GetRotatedAboutYRadians( float radiansToRotateAroundY ) const
{
	float r = GetLengthXZ();

	float currentRadians = GetAngleAboutYRadians();

	float newRadians = currentRadians + radiansToRotateAroundY;

	float newX = r * cosf(newRadians);
	float newZ = r * sinf(newRadians);

	return Vec3(newX, y, newZ);
}

//------------------------------------------------------------------------------------------------------------------------------
const Vec3 Vec3::GetRotatedAboutXDegrees( float degreesToRotateAroundX ) const
{
	float radiansToRotate = DegreesToRadians(degreesToRotateAroundX);
	Vec3 resultVec = GetRotatedAboutXRadians(radiansToRotate);	
	return resultVec;
}

//------------------------------------------------------------------------------------------------------------------------------
const Vec3 Vec3::GetRotatedAboutXRadians( float radiansToRotateAroundX ) const
{
	float r = GetLengthXZ();

	float currentRadians = GetAngleAboutYRadians();

	float newRadians = currentRadians + radiansToRotateAroundX;

	float newY = r * cosf(newRadians);
	float newZ = r * sinf(newRadians);

	return Vec3(x, newY, newZ);
}

//------------------------------------------------------------------------------------------------------------------------------
const Vec3 Vec3::GetNormalized() const
{
	if(GetLength() == NULL)
	{
		return Vec3(0.0f ,0.0f, 0.0f);
	}

	float xNorm = x / sqrtf(x*x + y*y + z*z);
	float yNorm = y / sqrtf(x*x + y*y + z*z);
	float zNorm = z / sqrtf(x*x + y*y + z*z);

	return Vec3(xNorm, yNorm, zNorm);
}

//------------------------------------------------------------------------------------------------------------------------------
std::string Vec3::GetAsString() const
{
	std::string string;
	string = std::to_string(x);
	string += ",";
	string += std::to_string(y);
	string += ",";
	string += std::to_string(z);

	return string;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC const Vec3 Vec3::GetComponentMin(const Vec3& min, const Vec3& max)
{
	Vec3 newMin;

	newMin.x = GetLowerValue(min.x, max.x);
	newMin.y = GetLowerValue(min.y, max.y);
	newMin.z = GetLowerValue(min.z, max.z);

	return newMin;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC const Vec3 Vec3::GetComponentMinXY(const Vec3& min, const Vec3& max)
{
	Vec3 newMin;

	newMin.x = GetLowerValue(min.x, max.x);
	newMin.y = GetLowerValue(min.y, max.y);

	return newMin;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC const Vec3 Vec3::GetComponentMax(const Vec3& min, const Vec3& max)
{
	Vec3 newMax;

	newMax.x = GetHigherValue(min.x, max.x);
	newMax.y = GetHigherValue(min.y, max.y);
	newMax.z = GetHigherValue(min.z, max.z);

	return newMax;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC const Vec3 Vec3::GetComponentMaxXY(const Vec3& min, const Vec3& max)
{
	Vec3 newMax;

	newMax.x = GetHigherValue(min.x, max.x);
	newMax.y = GetHigherValue(min.y, max.y);

	return newMax;
}

//------------------------------------------------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------------------------------------------------
const Vec3 Vec3::operator + ( const Vec3& vecToAdd ) const
{
	return Vec3( x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z ); 
}

//------------------------------------------------------------------------------------------------------------------------------
const Vec3 Vec3::operator-( const Vec3& vecToSubtract ) const
{
	return Vec3( x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z ); 
}

//------------------------------------------------------------------------------------------------------------------------------
const Vec3 Vec3::operator*( float uniformScale ) const
{
	return Vec3( x * uniformScale, y * uniformScale, z * uniformScale ); 
}

//------------------------------------------------------------------------------------------------------------------------------
const Vec3 Vec3::operator/( float inverseScale ) const
{
	return Vec3( x / inverseScale, y / inverseScale, z / inverseScale ); 
}

//------------------------------------------------------------------------------------------------------------------------------
void Vec3::SetLengthXY( float setLength )
{
	float newLength = setLength;
	float vectorAngle = atan2f(y, x);
	x = newLength * cosf(vectorAngle);
	y = newLength * sinf(vectorAngle);
	z = 0.f;
}

//------------------------------------------------------------------------------------------------------------------------------
void Vec3::Normalize()
{
	float invLength = 1 / sqrtf(x*x + y * y + z * z);

	x = x * invLength;
	y = y * invLength;
	z = z * invLength;
}

//------------------------------------------------------------------------------------------------------------------------------
const Vec3 Vec3::ClampVector( Vec3& toClamp, const Vec3& minBound, const Vec3& maxBound )
{
	toClamp.x = Clamp(toClamp.x, minBound.x, maxBound.x);
	toClamp.y = Clamp(toClamp.y, minBound.y, maxBound.y);
	toClamp.z = Clamp(toClamp.z, minBound.z, maxBound.z);

	return toClamp;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC const Vec3 Vec3::LerpVector(Vec3& toLerp, const Vec3& lerpDestination, float lerpPercent)
{
	Vec3 result;
	result = toLerp + lerpPercent * (lerpDestination - toLerp);
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------
void Vec3::operator+=( const Vec3& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y; 
	z += vecToAdd.z;
}

//------------------------------------------------------------------------------------------------------------------------------
void Vec3::operator-=( const Vec3& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
}

//------------------------------------------------------------------------------------------------------------------------------
void Vec3::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
}

//------------------------------------------------------------------------------------------------------------------------------
void Vec3::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor;
	y /= uniformDivisor;
	z /= uniformDivisor;
}

//------------------------------------------------------------------------------------------------------------------------------
void Vec3::operator=( const Vec3& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}

//------------------------------------------------------------------------------------------------------------------------------
const Vec3 operator*( float uniformScale, const Vec3& vecToScale )
{
	return Vec3( vecToScale.x * uniformScale, vecToScale.y * uniformScale, vecToScale.z * uniformScale);
}

//------------------------------------------------------------------------------------------------------------------------------
bool Vec3::operator==( const Vec3& compare ) const
{
	if(x == compare.x && y == compare.y && z == compare.z)
		return true;
	else
		return false;
}

//------------------------------------------------------------------------------------------------------------------------------
bool Vec3::operator!=( const Vec3& compare ) const
{
	if(x == compare.x && y == compare.y && z == compare.z)
		return false;
	else
		return true;
}
