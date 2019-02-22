#include "Rgba.hpp"
#include "Engine/Commons/EngineCommon.hpp"
#include <vector>
#include <string>

Rgba::Rgba( float initial_r, float initial_g, float initial_b, float inital_a )
	: r( initial_r )
	, g( initial_g )
	, b( initial_b )
	, a( inital_a )
{
}

Rgba::Rgba( const char* asText )
{
	SetFromText(asText);
}

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

void Rgba::SetFromBytes( unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte)
{
	r = static_cast<float>(redByte) / 255.f;
	g = static_cast<float>(greenByte) / 255.f;
	b = static_cast<float>(blueByte) / 255.f;
	a = static_cast<float>(alphaByte) / 255.f;
}


bool Rgba::operator!=( const Rgba& compare ) const
{
	if(r != compare.r || g != compare.g || b != compare.b || a != compare.a)
		return true;
	else
	{
		return false;
	}
}

bool Rgba::operator==( const Rgba& compare ) const
{
	if( r == compare.r && g == compare.g && b == compare.b && a == compare.a)
		return true;
	else
	{
		return false;
	}
}

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
