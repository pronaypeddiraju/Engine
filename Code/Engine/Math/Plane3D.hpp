#pragma once
#include "Engine/Math/Vec3.hpp"

struct Plane3D
{
	Plane3D();
	explicit Plane3D(Vec3 normal, float signedDistance);
	~Plane3D();

	Vec3 m_normal;
	float m_signedDistance;
};