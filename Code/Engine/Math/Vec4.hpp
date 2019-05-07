//------------------------------------------------------------------------------------------------------------------------------
#pragma once

struct Matrix44;

//------------------------------------------------------------------------------------------------------------------------------
struct Vec4
{
public:
	Vec4() {}
	~Vec4() {}

	Vec4( const Vec4& copyFrom );											// copy constructor (from another vec4)
	explicit Vec4( float initialX, float initialY, float initialZ, float initialW );		// explicit constructor (from x, y, z, w)
	explicit Vec4(const char* asText);

	//Statics
	const static Vec4	ZERO;
	const static Vec4	ONE;

	//Access Methods
	
	//Mutator methods
	void						SetFromText(const char* asText);
	
	// Operators
	const Vec4					operator+( const Vec4& vecToAdd ) const;					// vec2 + vec2
	const Vec4					operator-( const Vec4& vecToSubtract ) const;				// vec2 - vec2
	const Vec4					operator*( float uniformScale ) const;						// vec2 * float
	const Vec4					operator/( float inverseScale ) const;						// vec2 / float
	void						operator+=( const Vec4& vecToAdd );							// vec2 += vec2
	void						operator-=( const Vec4& vecToSubtract );					// vec2 -= vec2
	void						operator*=( const float uniformScale );						// vec2 *= float
	void						operator/=( const float uniformDivisor );					// vec2 /= float
	void						operator=( const Vec4& copyFrom );							// vec2 = vec2
	bool						operator==( const Vec4& compare ) const;					// vec2 == vec2
	bool						operator!=( const Vec4& compare ) const;					// vec2 != vec2

	friend const Vec4			operator*( float uniformScale, const Vec4& vecToScale );	// float * vec2
	void						operator*=(const Matrix44 matrix);

public: 
	float x;
	float y;
	float z;
	float w;
};