#pragma once

struct Rgba
{
public:
	//public variables
	float r = 1.0f;
	float g = 1.0f;
	float b = 1.0f;
	float a = 1.0f;

	const static Rgba WHITE;
	const static Rgba RED;
	const static Rgba GREEN;
	const static Rgba BLUE;
	const static Rgba BLACK;
	const static Rgba YELLOW;
	const static Rgba NONE;

private:
	//private variables

public:
	//public methods
	Rgba() {}	//default constructor
	~Rgba() {}	//destructor
	explicit Rgba( float initial_r, float initial_g, float initial_b, float inital_a = 1.0f);	//explicit constructor
	explicit Rgba(const char* asText);

	void			SetFromText(const char* asText);
	void			SetFromBytes( unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte = 255 );

	bool operator==( const Rgba& compare ) const;				// vec2 == vec2
	bool operator!=( const Rgba& compare ) const;

private:
	//private methods
};