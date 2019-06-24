//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Commons/EngineCommon.hpp"

//------------------------------------------------------------------------------------------------------------------------------
AABB3::AABB3()
{

}

//------------------------------------------------------------------------------------------------------------------------------
AABB3::AABB3( const Vec3& minBounds, const Vec3& maxBounds)
{
	//Setup the front face
	m_frontBottomLeft = minBounds;
	m_frontTopRight = Vec3(maxBounds.x, maxBounds.y, minBounds.z);
	m_frontTopLeft = Vec3(minBounds.x, maxBounds.y, minBounds.z);
	m_frontBottomRight = Vec3(maxBounds.x, minBounds.y, minBounds.z);

	Vec3 depthVec = Vec3(0.f, 0.f, (maxBounds.z - minBounds.z));

	//Setup the Back Face
	m_backBottomLeft = m_frontBottomLeft + depthVec;
	m_backBottomRight = m_frontBottomRight + depthVec;
	m_backTopLeft = m_frontTopLeft + depthVec;
	m_backTopRight = m_frontTopRight + depthVec;	

	m_center = Vec3(m_frontTopLeft.x + (m_frontTopRight.x - m_frontTopLeft.x) * 0.5f, m_frontBottomLeft.y + (m_frontTopLeft.y - m_frontBottomLeft.y) * 0.5f, m_frontBottomLeft.z + (m_backBottomLeft.z - m_frontBottomLeft.z) * 0.5f);
}

//------------------------------------------------------------------------------------------------------------------------------
AABB3::~AABB3()
{

}

//------------------------------------------------------------------------------------------------------------------------------
const STATIC AABB3 AABB3::UNIT_CUBE(Vec3(-0.5f, -0.5f, -0.5f), Vec3(0.5f, 0.5f, 0.5f));

//------------------------------------------------------------------------------------------------------------------------------
void AABB3::GetCornersForAABB3(Vec3* corners)
{
	//	Order
	//    5-----6
	//   /|    /|
	//  1-----2 |
	//  | 4---|-7
	//  |/    |/
	//  0-----3

	corners[0] = m_frontBottomLeft;
	corners[1] = m_frontTopLeft;
	corners[2] = m_frontTopRight;
	corners[3] = m_frontBottomRight;

	corners[4] = m_backBottomLeft;
	corners[5] = m_backTopLeft;
	corners[6] = m_backTopRight;
	corners[7] = m_backBottomRight;
}

//------------------------------------------------------------------------------------------------------------------------------
void AABB3::TranslatePointsBy(const Vec3& translation)
{
	m_frontBottomLeft += translation;
	m_frontTopLeft += translation;
	m_frontTopRight += translation;
	m_frontBottomRight += translation;

	m_backBottomLeft += translation;
	m_backBottomRight += translation;
	m_backTopLeft += translation;
	m_backTopRight += translation;
}

//------------------------------------------------------------------------------------------------------------------------------
void AABB3::TransfromUsingMatrix(const Matrix44& translation)
{
	m_frontBottomLeft = translation.TransformPosition3D(m_frontBottomLeft);
	m_frontTopLeft = translation.TransformPosition3D(m_frontTopLeft);
	m_frontTopRight = translation.TransformPosition3D(m_frontTopRight);
	m_frontBottomRight = translation.TransformPosition3D(m_frontBottomRight);

	m_backBottomLeft = translation.TransformPosition3D(m_backBottomLeft);
	m_backBottomRight = translation.TransformPosition3D(m_backBottomRight);
	m_backTopLeft = translation.TransformPosition3D(m_backTopLeft);
	m_backTopRight = translation.TransformPosition3D(m_backTopRight);
}
