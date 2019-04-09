//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/AABB2.hpp"
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
	m_backBottomLeft = m_frontBottomRight + depthVec;
	m_backBottomRight = m_frontBottomLeft + depthVec;
	m_backTopLeft = m_frontTopRight + depthVec;
	m_backTopRight = m_frontTopLeft + depthVec;	

	m_center = Vec3(m_frontTopLeft.x + (m_frontTopRight.x - m_frontTopLeft.x) * 0.5f, m_frontBottomLeft.y + (m_frontTopLeft.y - m_frontBottomLeft.y) * 0.5f, m_frontBottomLeft.z + (m_backBottomLeft.z - m_frontBottomLeft.z) * 0.5f);
}

//------------------------------------------------------------------------------------------------------------------------------
AABB3::~AABB3()
{

}

//------------------------------------------------------------------------------------------------------------------------------
const STATIC AABB3 AABB3::UNIT_CUBE(Vec3(-0.5f, -0.5f, -0.5f), Vec3(0.5f, 0.5f, 0.5f));
