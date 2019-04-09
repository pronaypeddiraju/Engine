//------------------------------------------------------------------------------------------------------------------------------
#pragma once

//------------------------------------------------------------------------------------------------------------------------------
struct IntRange
{
public:
	IntRange() {}
	~IntRange() {}

	IntRange( const IntRange& copyFrom );	
	explicit IntRange(int min, int max);
	explicit IntRange(const char* asText);

	const static IntRange ZeroToOne;
	const static IntRange Zero;
	const static IntRange One;

	void SetFromText(const char* asText);

public:
	int minInt;
	int maxInt;
};