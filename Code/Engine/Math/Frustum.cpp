//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Math/Frustum.hpp"
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/AABB3.hpp"

//------------------------------------------------------------------------------------------------------------------------------
bool Frustum::ContainsPoint(const Vec3& pos) const
{
	//If pos has +ve distance from all faces it is outside the frustum
	bool result = true;

 	for (int i = 0; i < 6; i++)
	{
		if (!m_planes[i].IsPointBehindPlane(pos))
		{
			result = false;
		}
	}

	return result;
}

//------------------------------------------------------------------------------------------------------------------------------
void Frustum::MakeFromAABB3(AABB3* box)
{
	//	Order
	//    5-----6
	//   /|    /|
	//  1-----2 |
	//  | 4---|-7
	//  |/    |/
	//  0-----3

	Vec3 corners[8];

	box->GetCornersForAABB3(&corners[0]);
	m_planes[FRUSTUM_LEFT] = Plane3D::MakeFromTriangleLHRule(corners[0], corners[4], corners[5]);
	m_planes[FRUSTUM_RIGHT] = Plane3D::MakeFromTriangleLHRule(corners[3], corners[6], corners[7]);
	m_planes[FRUSTUM_TOP] = Plane3D::MakeFromTriangleLHRule(corners[1], corners[5], corners[6]);
	m_planes[FRUSTUM_BOTTOM] = Plane3D::MakeFromTriangleLHRule(corners[0], corners[7], corners[4]);
	m_planes[FRUSTUM_FRONT] = Plane3D::MakeFromTriangleLHRule(corners[0], corners[1], corners[2]);
	m_planes[FRUSTUM_BACK] = Plane3D::MakeFromTriangleLHRule(corners[4], corners[7], corners[6]);

}

