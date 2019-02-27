//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/AABB2.hpp"

AABB3::AABB3()
{

}

AABB3::AABB3( const Vec3& frontMinBounds, const Vec3& frontMaxBounds, float boxDepth )
{
	//Setup the front face
	m_frontBottomLeft = frontMinBounds;
	m_frontTopRight = frontMaxBounds;
	m_frontTopLeft = Vec3(frontMinBounds.x, frontMaxBounds.y, frontMinBounds.z);
	m_frontBottomRight = Vec3(frontMaxBounds.x, frontMinBounds.y, frontMinBounds.z);

	Vec3 depthVec = Vec3(0.f, 0.f, boxDepth);

	//Setup the Back Face
	m_backBottomLeft = m_frontBottomLeft + depthVec;
	m_backBottomRight = m_frontBottomRight + depthVec;
	m_backTopLeft = m_frontTopLeft + depthVec;
	m_backTopRight = m_frontTopRight + depthVec;

	ConstructAllFaces();

}

const AABB2& AABB3::GetFrontFace() const
{
	return m_frontFace;
}

const AABB2& AABB3::GetBackFace() const
{
	return m_backFace;
}

const AABB2 & AABB3::GetTopFace() const
{
	return m_topFace;
}

const AABB2 & AABB3::GetBottomFace() const
{
	return m_bottomFace;
}

const AABB2 & AABB3::GetLeftFace() const
{
	return m_leftFace;
}

const AABB2 & AABB3::GetRightFace() const
{
	return m_rightFace;
}

void AABB3::ConstructAllFaces()
{
	//Front Face
	m_frontFace = AABB2(m_frontBottomLeft, m_frontTopRight);

	//Back Face
	m_backFace = AABB2(m_backBottomLeft, m_backTopRight);

	//Top Face
	m_topFace = AABB2(m_frontTopLeft, m_backTopRight);

	//Bottom Face
	m_bottomFace = AABB2(m_frontBottomLeft, m_backBottomRight);

	//Left Face
	m_leftFace = AABB2(m_backBottomLeft, m_frontTopLeft);

	//Right Face
	m_rightFace = AABB2(m_frontBottomRight, m_backTopRight);
}

AABB3::~AABB3()
{

}

