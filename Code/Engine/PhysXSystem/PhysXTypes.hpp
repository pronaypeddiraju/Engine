#pragma once

enum PhysXConvexMeshCookingTypes_T
{
	QUICKHULL
};

enum PhysXGeometryTypes_T
{
	BOX,
	CAPSULE,
	SPHERE,
	CONVEX_HULL
};

enum PhysXJointType_T
{
	SIMPLE_SPHERICAL,
	LIMITED_SPHERICAL,
	SIMPLE_FIXED,
	BREAKABLE_FIXED,
	D6_DAMPED,
	D6_SIMPLE
};