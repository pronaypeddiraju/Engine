#pragma once
#include "Engine/Math/Vec3.hpp"

struct Capsule3D
{
	Capsule3D();
	explicit Capsule3D(Vec3 start, Vec3 end, float radius = 1.f);
	~Capsule3D();

	Vec3 m_start;
	Vec3 m_end;
	float m_radius;
};