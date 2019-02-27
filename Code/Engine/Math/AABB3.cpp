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

void AABB3::ConstructAllFaces()
{

}

AABB3::~AABB3()
{

}

