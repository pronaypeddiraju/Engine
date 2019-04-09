#pragma once
#include "Engine/Math/Vec2.hpp"

//------------------------------------------------------------------------------------------------------------------------------
class SpriteDefenition
{
public:
	explicit SpriteDefenition(const Vec2& minUVs, const Vec2& maxUVs);
	~SpriteDefenition();

	void GetUVs(Vec2& minUVs, Vec2& maxUVs);

private:
	Vec2 m_uvAtBottomLeft = Vec2::ZERO;
	Vec2 m_uvAtTopright = Vec2::ONE;
};