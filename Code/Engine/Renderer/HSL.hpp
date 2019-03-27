#pragma once

//------------------------------------------------------------------------------------------------------------------------------
struct Vec3;
struct Rgba;

/*
//------------------------------------------------------------------------------------------------------------------------------
struct HSL
{
public:
	float m_H;
	float m_S;
	float m_L;

	HSL(float r, float g, float b)
	{
		SetHSLFromRGB(r, g, b);
	}

	HSL(float h, float s, float l)
	{
		m_H = h;
		m_S = s;
		m_L = l;
	}

	HSL			GetHSLFromRGBA( const Rgba& color , const Rgba& bg = Rgba::BLACK);
	Rgba		GetRGBAFromHSL( const HSL& hsl );
	void		SetHSLFromRGB( float r, float g, float b );
	Vec3		SetRGBAToRGB( const Rgba& color, const Rgba& bg = Rgba::BLACK);
	Vec3		HSLToRGB( HSL hsl );

	Vec3		m_rgb = Vec3::ZERO;

};
*/