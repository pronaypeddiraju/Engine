//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"

typedef unsigned int uint;

class Capsule2D;
class ConvexHull2D;
class Disc2D;
class OBB2;
class Plane2D;
struct AABB2;

struct RayHit2D
{
	Vec2 m_hitPoint;
	Vec2 m_impactNormal;
	float m_timeAtHit;
};

//------------------------------------------------------------------------------------------------------------------------------
struct Ray2D
{
	Ray2D();
	explicit Ray2D(Vec2 start, Vec2 direction);
	~Ray2D();

	Vec2	GetPointAtTime(float time) const;

	Vec2 m_start = Vec2::ZERO;	Vec2 m_direction = Vec2::LEFT;

	//Only for bit bucket based broad phase checks
	IntVec2		m_bitFieldsXY;
};

//Raycast functions for different types of objects we are raycasting to
uint Raycast(float *out, const Ray2D& ray, Disc2D const &disc);			// disc
uint Raycast(float *out, const Ray2D& ray, Capsule2D const &capsule);      // capsule
uint Raycast(float *out, const Ray2D& ray, Plane2D const &plane, float epsilon = 0.f);          // plane
uint Raycast(RayHit2D *out, const Ray2D& ray, ConvexHull2D const &hull, float epsilon = 0.f);		// convex hull

uint Raycast(float *out, Ray2D ray, OBB2 const &box);            // obb2
uint Raycast(float *out, Ray2D ray, AABB2 const &box);            // aabb2
