#pragma once
#include "Engine/Math/Vec3.hpp"

struct AABB2;
struct AABB3;
struct OBB3;
class OBB2;

struct Plane3D
{
	Plane3D();
	explicit Plane3D(Vec3 normal, float signedDistance);
	explicit Plane3D(Vec3 normal, Vec3 pointOnPlane);
	~Plane3D();

	bool			IsPointInFrontOfPlane(const Vec3& position) const;
	bool			IsPointBehindPlane(const Vec3& position) const;

// 	static Plane3D	MakePlaneFromBox(const AABB2& box);
// 	static Plane3D	MakePlaneFromBox(const AABB3& box);
// 	static Plane3D	MakePlaneFromBox(const OBB2& box);
// 	static Plane3D	MakePlaneFromBox(const OBB3& box);
	static Plane3D	MakeFromTriangleLHRule(const Vec3& point1, const Vec3& point2, const Vec3& point3);
	
	Vec3 m_normal;
	float m_signedDistance;
};