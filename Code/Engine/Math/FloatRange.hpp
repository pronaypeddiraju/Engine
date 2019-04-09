//------------------------------------------------------------------------------------------------------------------------------
#pragma once

//------------------------------------------------------------------------------------------------------------------------------
struct FloatRange
{
public:
	FloatRange() {}
	~FloatRange() {}

	FloatRange( const FloatRange& copyFrom );	
	explicit FloatRange(float min, float max);
	explicit FloatRange(const char* asText);

	const static FloatRange ZeroToOne;

	void SetFromText(const char* asText);

public:
	float minFloat;
	float maxFloat;
};