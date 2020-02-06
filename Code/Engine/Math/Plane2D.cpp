//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Math/Plane2D.hpp"
#include "Engine/Math/MathUtils.hpp"

//------------------------------------------------------------------------------------------------------------------------------
Plane2D::Plane2D()
{

}

//------------------------------------------------------------------------------------------------------------------------------
Plane2D::Plane2D( Vec2 normal, float distanceToOrigin )
{
	m_normal = normal;
	m_signedDistance = distanceToOrigin;
}

//------------------------------------------------------------------------------------------------------------------------------
Plane2D::Plane2D(Vec2 normal, Vec2 pointOnPlane)
{
	//Use DotProduct(p, normal) = signedDistance to find the signed distance for this plane
	m_signedDistance = GetDotProduct(normal, pointOnPlane);
	m_normal = normal;
}

//------------------------------------------------------------------------------------------------------------------------------
float Plane2D::GetDistance( const Vec2& point ) const
{
	float distance = GetDotProduct(point, m_normal);
	distance -= m_signedDistance;
	return distance;
}

//------------------------------------------------------------------------------------------------------------------------------
bool Plane2D::operator==(const Plane2D& compare) const
{
	if (m_signedDistance == compare.m_signedDistance && m_normal == compare.m_normal)
		return true;
	else
		return false;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC Plane2D Plane2D::AtPosition( const Vec2& pos, const Vec2& normal ) 
{
	Plane2D p; 
	p.m_normal = normal; 
	//p.m_distance = -GetDotProduct( pos, normal );    // C4
	
	p.m_signedDistance = GetDotProduct( pos, normal );  // Squirrel

	return p;
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC Plane2D Plane2D::FromPoints( const Vec2& p0, const Vec2& p1 )
{
	Plane2D p;
	
	Vec2 displacement = p1 - p0;
	Vec2 tangent = displacement.GetNormalized();
	p.m_normal = tangent.GetRotated90Degrees() * -1.f;
	p.m_signedDistance = -GetDotProduct( p1, p.m_normal );    // C4

	return p;
}
