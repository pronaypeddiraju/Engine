/*
#include "Engine/Renderer/HSL.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/Rgba.hpp"

HSL HSL::GetHSLFromRGBA(const Rgba& color, const Rgba& bg)
{
	Vec3 m_rgb = SetRGBAToRGB(color, bg);
	SetHSLFromRGB(m_rgb.x, m_rgb.y, m_rgb.z);
}
Rgba HSL::GetRGBAFromHSL(const HSL& hsl)
{
	Vec3 rgb = HSLToRGB(hsl);
	return Rgba(rgb.x, rgb.y, rgb.z, 1.f);
}

void HSL::SetHSLFromRGB( float r, float g, float b )
{
	//float rb = (rgb.R / 255.0f);
	//float gb = (rgb.G / 255.0f);
	//float bb = (rgb.B / 255.0f);

	float min = GetLowerValue(GetHigherValue(r, g), b);
	float max = GetHigherValue(GetHigherValue(r, g), b);
	float delta = max - min;

	m_L = (max + min) / 2;

	if (delta == 0)
	{
		m_H = 0;
		m_S = 0.0f;
	}
	else
	{
		m_S = (m_L <= 0.5) ? (delta / (max + min)) : (delta / (2 - max - min));
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

		m_H = (int)(hue * 360);
	}
}

Vec3 HSL::SetRGBAToRGB(const Rgba& color, const Rgba& bg)
{
	float alpha = color.a;

	return Vec3(
		(1 - alpha) * bg.r + alpha * color.r,
		(1 - alpha) * bg.g + alpha * color.g,
		(1 - alpha) * bg.b + alpha * color.b
	);
}

Vec3 HSL::HSLToRGB(HSL hsl) 
{
	float r = 0.f;
	float g = 0.f;
	float b = 0.f;

	if (hsl.m_S == 0)
	{
		r = g = b = (unsigned char)(hsl.m_L * 255);
	}
	else
	{
		float v1, v2;
		float hue = (float)hsl.m_H / 360;

		v2 = (hsl.m_L < 0.5) ? (hsl.m_L * (1 + hsl.m_S)) : ((hsl.m_L + hsl.m_S) - (hsl.m_L * hsl.m_S));
		v1 = 2 * hsl.m_L - v2;

		r = HueToRGB(v1, v2, hue + (1.0f / 3));
		g = HueToRGB(v1, v2, hue);
		b = HueToRGB(v1, v2, hue - (1.0f / 3));
	}

	return Vec3(r, g, b);
}

static float HueToRGB(float v1, float v2, float vH) {
	if (vH < 0)
		vH += 1;

	if (vH > 1)
		vH -= 1;

	if ((6 * vH) < 1)
		return (v1 + (v2 - v1) * 6 * vH);

	if ((2 * vH) < 1)
		return v2;

	if ((3 * vH) < 2)
		return (v1 + (v2 - v1) * ((2.0f / 3) - vH) * 6);

	return v1;
}
*/