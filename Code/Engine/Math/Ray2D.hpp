//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/Vec2.hpp"

typedef unsigned int uint;

struct Disc2D;
struct Capsule2D;
struct Plane2D;
struct OBB2;
struct AABB2;

//------------------------------------------------------------------------------------------------------------------------------
struct Ray2D
{
	Ray2D();
	explicit Ray2D(Vec2 start, Vec2 direction);
	~Ray2D();

	Vec2	GetPointAtTime(float time);

	Vec2 m_start = Vec2::ZERO;
	Vec2 m_direction = Vec2::LEFT;
};

//Raycast functions for different types of objects we are raycasting to
uint Raycast(float *out, Ray2D ray, Disc2D const &disc);			// disc
uint Raycast(float *out, Ray2D ray, Capsule2D const &capsule);      // capsule
uint Raycast(float *out, Ray2D ray, Plane2D const &plane);          // plane
uint Raycast(float *out, Ray2D ray, OBB2 const &box);            // obb2
uint Raycast(float *out, Ray2D ray, AABB2 const &box);            // aabb2
