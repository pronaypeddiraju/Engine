//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Plane3D.hpp"

//------------------------------------------------------------------------------------------------------------------------------
enum eFrustumFace 
{
	FRUSTUM_RIGHT = 0,
	FRUSTUM_TOP,
	FRUSTUM_FRONT,
	FRUSTUM_LEFT,
	FRUSTUM_BOTTOM,
	FRUSTUM_BACK,

	FRUSTUM_SIDE_COUNT,
};

//------------------------------------------------------------------------------------------------------------------------------
struct Frustum
{
public:
	bool ContainsPoint(const Vec3& pos) const;
	void MakeFromAABB3(AABB3* box);

public:
	Plane3D m_planes[FRUSTUM_SIDE_COUNT];
};