#pragma once
#include <string>
//------------------------------------------------------------------------------------------------------------------------------
struct Vec2;

//------------------------------------------------------------------------------------------------------------------------------
struct Vec3
{
public:
	// Construction/Destruction
	~Vec3() {}																// destructor: do nothing (for speed)
	Vec3() {}																// default constructor: do nothing (for speed)
	Vec3( const Vec3& copyFrom );											// copy constructor (from another vec3)
	Vec3( const Vec2& copyFrom );
	explicit Vec3( float initialX, float initialY, float initialZ );		// explicit constructor (from x, y, z)
	explicit Vec3(const char* asText);

	//Static Vectors
	const static Vec3	ZERO;
	const static Vec3	ONE;
	const static Vec3	FRONT;
	const static Vec3	FORWARD;
	const static Vec3	BACK;
	const static Vec3	LEFT;
	const static Vec3	RIGHT;
	const static Vec3	UP;
	const static Vec3	DOWN;

	static const Vec3			GetComponentMin(const Vec3& min, const Vec3& max);
	static const Vec3			GetComponentMinXY(const Vec3& min, const Vec3& max);
	static const Vec3			GetComponentMax(const Vec3& min, const Vec3& max);
	static const Vec3			GetComponentMaxXY(const Vec3& min, const Vec3& max);

	//Access Methods
	float				GetLength() const;
	float				GetLengthXY() const;
	float				GetLengthXZ() const;
	float				GetLengthYZ() const;
	float				GetLengthSquared() const;
	float				GetLengthSquaredXY() const;
	float				GetAngleAboutZDegrees() const;
	float				GetAngleAboutZRadians() const;
	float				GetAngleAboutYDegrees() const;
	float				GetAngleAboutYRadians() const;
	float				GetAngleAboutXDegrees() const;
	float				GetAngleAboutXRadians() const;
	float				GetAngleDegreesXY() const;
	float				GetAngleRadiansXY() const;
	const Vec3			GetRotatedAboutZDegrees(float degreesToRotateAroundZ) const;
	const Vec3			GetRotatedAboutZRadians(float radiansToRotateAroundZ) const;
	const Vec3			GetRotatedAboutYDegrees(float degreesToRotateAroundY) const;
	const Vec3			GetRotatedAboutYRadians(float radiansToRotateAroundY) const;
	const Vec3			GetRotatedAboutXDegrees(float degreesToRotateAroundX) const;
	const Vec3			GetRotatedAboutXRadians(float radiansToRotateAroundX) const;
	const Vec3			GetNormalized() const;
	std::string			GetAsString() const;

	//Mutator methods
	void				SetFromText(const char* asText);
	void				ClampLengthXY(float maxLength);
	void				SetLengthXY(float setLength);
	void				Normalize();
	const Vec3			ClampVector(Vec3& toClamp, const Vec3& minBound, const Vec3& maxBound);

	// Operators
	const Vec3			operator+( const Vec3& vecToAdd ) const;					// vec3 + vec3
	const Vec3			operator-( const Vec3& vecToSubtract ) const;				// vec3 - vec3
	const Vec3			operator*( float uniformScale ) const;						// vec3 * float
	const Vec3			operator/( float inverseScale ) const;						// vec3 / float
	void				operator+=( const Vec3& vecToAdd );							// vec3 += vec3
	void				operator-=( const Vec3& vecToSubtract );					// vec3 -= vec3
	void				operator*=( const float uniformScale );						// vec3 *= float
	void				operator/=( const float uniformDivisor );					// vec3 /= float
	void				operator=( const Vec3& copyFrom );							// vec3 = vec3
	bool				operator==( const Vec3& compare ) const;					// vec3 == vec3
	bool				operator!=( const Vec3& compare ) const;					// vec3 != vec3

	friend const Vec3	operator*( float uniformScale, const Vec3& vecToScale );	// float * vec3

public: 
	float x;
	float y;
	float z;
};


