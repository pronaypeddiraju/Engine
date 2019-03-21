//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Math/Segment2D.hpp"
#include "Engine/Math/MathUtils.hpp"

//------------------------------------------------------------------------------------------------------------------------------

Segment2D::Segment2D( Vec2 start, Vec2 end )
{
	m_start = start;
	m_end = end;
}

Segment2D::Segment2D()
{

}

Segment2D::~Segment2D()
{

}

Vec2 Segment2D::GetCenter() const
{
	Vec2 disp = m_start - m_end;
	Vec2 norm = disp.GetNormalized();
	float length = disp.GetLength();

	Vec2 center = m_end + norm * 0.5f * length;
	return center;
}

Vec2 Segment2D::GetClosestPoint( Vec2 pos ) const
{
	Vec2 closestPoint = GetClosestPointOnLineSegment2D(pos, m_start, m_end);
	return closestPoint;
}

