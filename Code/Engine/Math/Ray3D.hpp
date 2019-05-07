//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/Vec3.hpp"

typedef unsigned int uint;

struct Sphere;
struct Capsule3D;
struct Plane3D;
struct OBB3;

//------------------------------------------------------------------------------------------------------------------------------
struct Ray3D
{
	Ray3D();
	explicit Ray3D(Vec3 start, Vec3 direction);
	~Ray3D();

	Vec3	GetPointAtTime(float time);

	Vec3 m_start = Vec3::ZERO;
	Vec3 m_direction = Vec3::FORWARD;
};

//Raycast functions for different types of objects we are raycasting to
uint Raycast(float *out, Ray3D ray, Sphere const &sphere);          // sphere
uint Raycast(float *out, Ray3D ray, Capsule3D const &capsule);      // capsule
uint Raycast(float *out, Ray3D ray, Plane3D const &plane);          // plane
uint Raycast(float *out, Ray3D ray, OBB3 const &bounds);            // obb3