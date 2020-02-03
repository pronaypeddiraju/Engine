//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Commons/EngineCommon.hpp"

//------------------------------------------------------------------------------------------------------------------------------
class Disc2D
{
public:
	Disc2D();
	explicit Disc2D(const Vec2& centre, float radius = 1.f);
	~Disc2D();

	const Vec2&				GetCentre() const;
	float					GetRadius() const;

	void					TranslateByVector(const Vec2& translation);

private:
	Vec2					m_discCentre = Vec2::ZERO;
	float					m_radius;
};
