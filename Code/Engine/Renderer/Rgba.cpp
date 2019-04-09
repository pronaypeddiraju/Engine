//------------------------------------------------------------------------------------------------------------------------------
#include "Rgba.hpp"
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec3.hpp"
#include <vector>
#include <string>

//------------------------------------------------------------------------------------------------------------------------------
Rgba::Rgba( float initial_r, float initial_g, float initial_b, float inital_a )
	: r( initial_r )
	, g( initial_g )
	, b( initial_b )
	, a( inital_a )
{
}

//------------------------------------------------------------------------------------------------------------------------------
Rgba::Rgba( const char* asText )
{
	SetFromText(asText);
}

//------------------------------------------------------------------------------------------------------------------------------
void Rgba::SetFromText( const char* asText )
{
	std::vector< std::string> splitStrings = SplitStringOnDelimiter(asText, ',');
	if(splitStrings.size() != 3 && splitStrings.size() != 4)
	{
		ERROR_AND_DIE("ERROR: Data from Vec2 SetFromText did not recieve 2 string components");
	}
	else
	{
		if(splitStrings.size() == 3)
		{
			r = static_cast<float>(atof(splitStrings[0].c_str())) / 255.f;
			g = static_cast<float>(atof(splitStrings[1].c_str())) / 255.f;
			b = static_cast<float>(atof(splitStrings[2].c_str())) / 255.f;
			a = 1.f;
		}
		else
		{
			r = static_cast<float>(atof(splitStrings[0].c_str())) / 255.f;
			g = static_cast<float>(atof(splitStrings[1].c_str())) / 255.f;
			b = static_cast<float>(atof(splitStrings[2].c_str())) / 255.f;
			a = static_cast<float>(atof(splitStrings[3].c_str())) / 255.f;
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void Rgba::SetFromBytes( unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte)
{
	r = static_cast<float>(redByte) / 255.f;
	g = static_cast<float>(greenByte) / 255.f;
	b = static_cast<float>(blueByte) / 255.f;
	a = static_cast<float>(alphaByte) / 255.f;
}

//------------------------------------------------------------------------------------------------------------------------------
void Rgba::LerpRGBA( Rgba& currentColor, const Rgba& startColor, const Rgba& endColor, float blendFraction )
{
	currentColor.r = RangeMapFloat(blendFraction, 0.f, 1.f, startColor.r, endColor.r);
	currentColor.g = RangeMapFloat(blendFraction, 0.f, 1.f, startColor.g, endColor.g);
	currentColor.b = RangeMapFloat(blendFraction, 0.f, 1.f, startColor.b, endColor.b);
	currentColor.a = RangeMapFloat(blendFraction, 0.f, 1.f, startColor.a, endColor.a);
}

//------------------------------------------------------------------------------------------------------------------------------
void Rgba::LerpRGB( Rgba& currentColor, const Rgba& startColor, const Rgba& endColor, float blendFraction )
{
	currentColor.r = RangeMapFloat(blendFraction, 0.f, 1.f, startColor.r, endColor.r);
	currentColor.g = RangeMapFloat(blendFraction, 0.f, 1.f, startColor.g, endColor.g);
	currentColor.b = RangeMapFloat(blendFraction, 0.f, 1.f, startColor.b, endColor.b);
}

//------------------------------------------------------------------------------------------------------------------------------
Rgba Rgba::operator*( float multiplier ) const
{
	Rgba color = Rgba(r * multiplier, g * multiplier, b * multiplier, a * multiplier);
	return color;
}

/*
Vec3 Rgba::GetHSLFromRGBA( Rgba& currentColor, const Rgba& bg )
{
	Vec3 m_rgb = SetRGBAToRGB(currentColor, bg);
	SetHSLFromRGB(m_rgb);
	return m_HSLColor;
}

Vec3 Rgba::SetRGBAToRGB(const Rgba& color, const Rgba& bg)
{
	float alpha = color.a;

	return Vec3(
		(1 - alpha) * bg.r + alpha * color.r,
		(1 - alpha) * bg.g + alpha * color.g,
		(1 - alpha) * bg.b + alpha * color.b
	);
}

void Rgba::SetHSLFromRGB( Vec3 rgb )
{
	//float rb = (rgb.R / 255.0f);
	//float gb = (rgb.G / 255.0f);
	//float bb = (rgb.B / 255.0f);

	float r = rgb.x;
	float g = rgb.y;
	float b = rgb.z;

	float H;
	float S;
	float L;

	float min = GetLowerValue(GetHigherValue(r, g), b);
	float max = GetHigherValue(GetHigherValue(r, g), b);
	float delta = max - min;

	L = (max + min) / 2;

	if (delta == 0)
	{
		H = 0.f;
		S = 0.0f;
	}
	else
	{
		S = (L <= 0.5) ? (delta / (max + min)) : (delta / (2 - max - min));
		float hue;

		if (r == max)
		{
			hue = ((g - b) / 6) / delta;
		}
		else if (g == max)
		{
			hue = (1.0f / 3) + ((b - r) / 6) / delta;
		}
		else
		{
			hue = (2.0f / 3) + ((r - g) / 6) / delta;
		}

		if (hue < 0)
			hue += 1;
		if (hue > 1)
			hue -= 1;

		H = (int)(hue * 360);
	}

	m_HSLColor = Vec3(H,S,L);
}
*/

//------------------------------------------------------------------------------------------------------------------------------
bool Rgba::operator!=( const Rgba& compare ) const
{
	if(r != compare.r || g != compare.g || b != compare.b || a != compare.a)
		return true;
	else
	{
		return false;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
bool Rgba::operator==( const Rgba& compare ) const
{
	if( r == compare.r && g == compare.g && b == compare.b && a == compare.a)
		return true;
	else
	{
		return false;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
const STATIC Rgba Rgba::WHITE(1.f,1.f, 1.f, 1.f);
const STATIC Rgba Rgba::RED(1.f,0.f, 0.f, 1.f);
const STATIC Rgba Rgba::GREEN(0.f,1.f, 0.f, 1.f);
const STATIC Rgba Rgba::BLUE(0.f,0.f, 1.f, 1.f);
const STATIC Rgba Rgba::BLACK(0.f,0.f, 0.f, 1.f);
const STATIC Rgba Rgba::YELLOW(1.f, 1.f, 0.f, 1.f);
const STATIC Rgba Rgba::DARK_GREY(0.6f, 0.6f, 0.6f, 1.f);
const STATIC Rgba Rgba::ORANGE(1.f, 0.65f, 0.f, 1.f);
const STATIC Rgba Rgba::MAGENTA(1.f, 0.f, 1.f, 1.f);
const STATIC Rgba Rgba::NONE(0.f,0.f, 0.f, 0.f);
//------------------------------------------------------------------------------------------------------------------------------