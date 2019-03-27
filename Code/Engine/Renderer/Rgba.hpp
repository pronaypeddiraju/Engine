#pragma once
//------------------------------------------------------------------------------------------------------------------------------
struct Vec3;

//------------------------------------------------------------------------------------------------------------------------------
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
	const static Rgba ORANGE;
	const static Rgba DARK_GREY;
	const static Rgba MAGENTA;

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

	static void		LerpRGBA( Rgba& currentColor, const Rgba& startColor, const Rgba& endColor, float blendFraction );
	static void		LerpRGB( Rgba& currentColor, const Rgba& startColor, const Rgba& endColor, float blendFraction );

	//Vec3 GetHSLFromRGBA( Rgba& currentColor, const Rgba& bg);

	bool operator==( const Rgba& compare ) const;				// vec2 == vec2
	bool operator!=( const Rgba& compare ) const;

private:
	//private methods
	//Vec3			SetRGBAToRGB( const Rgba& color, const Rgba& bg );
	//void			SetHSLFromRGB( Vec3 rgb );

	//Vec3			m_HSLColor;
};