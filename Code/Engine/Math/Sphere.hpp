#pragma once
#include "Engine/Math/Vec3.hpp"

struct Sphere
{
	Sphere();
	explicit Sphere(Vec3 point, float radius = 1.f);
	~Sphere();

	Vec3 m_point;
	float m_radius;
};