//------------------------------------------------------------------------------------------------------------------------------
#pragma once
//Engine Systems
#include "Engine/Math/Vec2.hpp"

//------------------------------------------------------------------------------------------------------------------------------
struct Transform2 
{
public:
	explicit Transform2(const Vec2& position, float rotation, const Vec2& scale);
	explicit Transform2(const Vec2& position);
	explicit Transform2(const Vec2& position, float rotation);
	explicit Transform2(const Vec2& position, const Vec2& scale);
	Transform2();
	~Transform2();

public:
	Vec2		m_position = Vec2::ZERO;
	float		m_rotation = 0.f;
	Vec2		m_scale = Vec2::ONE;
};