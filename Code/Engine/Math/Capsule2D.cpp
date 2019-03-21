//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Math/Capsule2D.hpp"
//Engine Systems
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"

//------------------------------------------------------------------------------------------------------------------------------
Capsule2D::Capsule2D()
{

}

Capsule2D::Capsule2D( Vec2 pos )
{
	m_start = pos;
	m_end = pos;

	m_radius = 0.f;
	m_rotationDegrees = 0.f;
}

Capsule2D::Capsule2D( Vec2 center, float radius )
{
	m_start = center;
	m_end = center;
	m_radius = radius;

	m_rotationDegrees = 0.f;
}

Capsule2D::Capsule2D( Vec2 p0, Vec2 p1, float radius )
{
	m_start = p0;
	m_end = p1;
	m_radius = radius;

	m_rotationDegrees = Vec2(m_start - m_end).GetAngleDegrees();
}

Capsule2D::~Capsule2D()
{

}

void Capsule2D::SetPosition( Vec2 pos )
{
	Vec2 disp = m_start - m_end;
	float length = disp.GetLength();
	Vec2 normal = disp.GetNormalized();
	Vec2 invNormal = normal * -1.f;

	float halfLength = (length * 0.5f);

	m_start = pos + normal * halfLength;
	m_end = pos + invNormal * halfLength;
}

void Capsule2D::SetPositions( Vec2 p0, Vec2 p1 )
{
	m_start = p0;
	m_end = p1;
}

void Capsule2D::Translate( Vec2 offset )
{
	m_start += offset;
	m_end += offset;
}

bool Capsule2D::Contains( Vec2 worldPoint )
{
	return IsPointInCapsule2D(worldPoint, m_start, m_end, m_radius);
}

Vec2 Capsule2D::GetClosestPoint( Vec2 worldPoint )
{
	Vec2 closestPoint = GetClosestPointOnCapsule2D(worldPoint, m_start, m_end, m_radius);
	return closestPoint;
}

Vec2 Capsule2D::GetCenter() const
{
	Vec2 disp = m_start - m_end;
	Vec2 norm = disp.GetNormalized();
	float distance = disp.GetLength();

	return m_end + (norm * distance * 0.5f);
}

AABB2 Capsule2D::GetBoundingAABB() const
{
	Vec2 minBounds = Vec2(GetLowerValue(m_start.x, m_end.x), GetLowerValue(m_start.y, m_end.y));
	Vec2 maxBounds = Vec2(GetHigherValue(m_start.x, m_end.x), GetHigherValue(m_start.y, m_end.y));

	//Use radius to make sure the bounding box is correct
	minBounds.x -= m_radius;
	minBounds.y -= m_radius;

	maxBounds.x += m_radius;
	maxBounds.y += m_radius;
	
	return AABB2(minBounds, maxBounds);
}

