#pragma once

//-----------------------------------------------------------------------------------------------
struct Vec3
{
public:
	// Construction/Destruction
	~Vec3() {}																// destructor: do nothing (for speed)
	Vec3() {}																// default constructor: do nothing (for speed)
	Vec3( const Vec3& copyFrom );											// copy constructor (from another vec3)
	explicit Vec3( float initialX, float initialY, float initialZ );		// explicit constructor (from x, y, z)
	explicit Vec3(const char* asText);

	//Access Methods
	float				GetLength() const;
	float				GetLengthXY() const;
	float				GetLengthSquared() const;
	float				GetLengthSquaredXY() const;
	float				GetAngleAboutZDegrees() const;
	float				GetAngleAboutZRadians() const;
	float				GetAngleDegreesXY() const;
	float				GetAngleRadiansXY() const;
	const Vec3			GetRotatedAboutZDegrees(float degreesToRotateAroundZ) const;
	const Vec3			GetRotatedAboutZRadians(float radiansToRotateAroundZ) const;

	//Mutator methods
	void				SetFromText(const char* asText);
	void				ClampLengthXY(float maxLength);
	void				SetLengthXY(float setLength);
	
	// Operators
	const Vec3 operator+( const Vec3& vecToAdd ) const;			// vec3 + vec3
	const Vec3 operator-( const Vec3& vecToSubtract ) const;	// vec3 - vec3
	const Vec3 operator*( float uniformScale ) const;			// vec3 * float
	const Vec3 operator/( float inverseScale ) const;			// vec3 / float
	void operator+=( const Vec3& vecToAdd );					// vec3 += vec3
	void operator-=( const Vec3& vecToSubtract );				// vec3 -= vec3
	void operator*=( const float uniformScale );				// vec3 *= float
	void operator/=( const float uniformDivisor );				// vec3 /= float
	void operator=( const Vec3& copyFrom );						// vec3 = vec3
	bool operator==( const Vec3& compare ) const;				// vec3 == vec3
	bool operator!=( const Vec3& compare ) const;				// vec3 != vec3

	friend const Vec3 operator*( float uniformScale, const Vec3& vecToScale );	// float * vec3

public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	float x;
	float y;
	float z;
};


