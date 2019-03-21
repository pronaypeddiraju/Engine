//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Math/Plane2D.hpp"
#include "Engine/Math/MathUtils.hpp"

//------------------------------------------------------------------------------------------------------------------------------
Plane2D::Plane2D()
{

}

Plane2D::Plane2D( Vec2 normal, float distanceToOrigin )
{
	m_normal = normal;
	m_distance = distanceToOrigin;
}

float Plane2D::GetDistance( Vec2 point ) const
{
	float distance = GetDotProduct(point, m_normal);
	distance -= m_distance;
	return distance;
}

STATIC Plane2D Plane2D::AtPosition( Vec2 pos, Vec2 normal ) 
{
	Plane2D p; 
	p.m_normal = normal; 
	//p.m_distance = -GetDotProduct( pos, normal );    // C4
	
	p.m_distance = GetDotProduct( pos, normal );  // Squirrel

	return p;
}

STATIC Plane2D Plane2D::FromPoints( Vec2 p0, Vec2 p1 )
{
	Plane2D p;
	
	Vec2 displacement = p1 - p0;
	Vec2 tangent = displacement.GetNormalized();
	p.m_normal = tangent.GetRotated90Degrees() * -1.f;
	p.m_distance = -GetDotProduct( p1, p.m_normal );    // C4

	return p;
}
