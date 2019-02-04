#pragma once

#include "Engine/Commons/EngineCommon.hpp"

class Disc2D
{
public:
	explicit Disc2D(const Vec2& centre, float radius = 1.f);
	~Disc2D();

	const Vec2&	GetCentre() const;
	float		GetRadius();

private:
	Vec2	m_discCentre = Vec2::ZERO;
	float	m_radius;
};
