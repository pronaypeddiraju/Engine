//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Plane2D.hpp"

//------------------------------------------------------------------------------------------------------------------------------
OBB2::OBB2()
{

}

OBB2::OBB2( Vec2 center, Vec2 size /*= Vec2::ZERO*/, float rotationDegrees /*= 0.0f */ )
{
	m_center = center;
	m_halfExtents = size * 0.5f;
	
	SetRotation(rotationDegrees);
}

OBB2::OBB2( AABB2 const &aabb )
{
	m_right = Vec2( 1.0f, 0.0f ); 
	m_up = Vec2( 0.0f, 1.0f ); 
	m_center = (aabb.m_minBounds + aabb.m_maxBounds) * 0.5f;
	m_halfExtents = (aabb.m_maxBounds - aabb.m_minBounds) * 0.5f; 
}

void OBB2::Translate( Vec2 offset )
{
	m_center += offset;
}

void OBB2::MoveTo( Vec2 position )
{
	m_center = position;
}

void OBB2::RotateBy( float degrees )
{
	m_right.RotateDegrees(degrees);
	m_up = m_right.GetRotated90Degrees();
}

void OBB2::SetRotation( float degrees )
{
	float cosDeg = CosDegrees(degrees);
	float sinDeg = SinDegrees(degrees);

	m_right = Vec2(cosDeg, sinDeg);
	m_up = m_right.GetRotated90Degrees();
}

void OBB2::SetSize( Vec2 newSize )
{
	m_halfExtents = newSize * 0.5f;
}

Vec2 OBB2::ToLocalPoint( Vec2 worldPoint ) const
{
	Vec2 disp = worldPoint - GetCenter();  

	// world i = m_right;
	// world j = m_up
	float localI = GetDotProduct( disp, GetRight() ); 
	float localJ = GetDotProduct( disp, GetUp() ); 

	return Vec2( localI, localJ ); 
}

Vec2 OBB2::ToWorldPoint( Vec2 localPoint ) const
{
	// world i = m_right;
	// world j = m_up

	return localPoint.x * GetRight() + localPoint.y * GetUp()    // Rotate
		+ GetCenter();                                           // Translate
}

void OBB2::GetPlanes( Plane2D* out ) const
{
	//Populate the 4 planes
	out[0] = Plane2D::AtPosition(GetTopLeft(), m_up);
	out[1] = Plane2D::AtPosition(GetTopRight(), m_right);
	out[2] = Plane2D::AtPosition(GetBottomRight(), m_up * -1.f);
	out[3] = Plane2D::AtPosition(GetBottomLeft(), m_right * -1.f);
}

void OBB2::GetCorners( Vec2* out ) const
{
	//Populate the 4 corners (order of planes?)
	out[0] = GetTopLeft();
	out[1] = GetTopRight();
	out[2] = GetBottomRight();
	out[3] = GetBottomLeft();
}

bool OBB2::Contains( Vec2 worldPoint ) const
{
	Vec2 localPoint = ToLocalPoint( worldPoint ); 
	Vec2 absLocalPoint = Vec2(abs(localPoint.x), abs(localPoint.y)); 
	return (absLocalPoint.x < m_halfExtents.x) && (absLocalPoint.y < m_halfExtents.y); 
}

Vec2 OBB2::GetClosestPoint( Vec2 worldPoint ) const
{
	Vec2 localPoint = ToLocalPoint( worldPoint ); 
	Vec2 clampedPoint = localPoint.ClampVector( localPoint, m_halfExtents * -1.f, m_halfExtents ); 

	return ToWorldPoint( clampedPoint ); 
}

bool OBB2::Intersects( OBB2 const &other ) const
{
	Plane2D planesOfThis[4];    // p0
	Plane2D planesOfOther[4];   // p1

	Vec2 cornersOfThis[4];     // c0
	Vec2 cornersOfOther[4];    // c1

	GetPlanes( planesOfThis ); 
	GetCorners( cornersOfThis ); 

	other.GetPlanes( planesOfOther ); 
	other.GetCorners( cornersOfOther ); 

	int inFrontOfThis = 0;
	int inFrontOfOther = 0;

	for(int planeIndex = 0; planeIndex < 4;planeIndex++)
	{
		Plane2D const &thisPlane = planesOfThis[planeIndex];
		Plane2D const &otherPlane = planesOfOther[planeIndex];

		inFrontOfThis = 0;
		inFrontOfOther = 0;

		for(int cornerIndex = 0; cornerIndex < 4; cornerIndex++)
		{
			Vec2 const &cornerOfThis = cornersOfThis[cornerIndex]; 
			Vec2 const &cornerOfOther = cornersOfOther[cornerIndex];

			float otherFromThis = thisPlane.GetDistance( cornerOfOther ); 
			float thisFromOther = otherPlane.GetDistance( cornerOfThis ); 

			inFrontOfThis += (otherFromThis >= 0.0f) ? 1 : 0; 
			inFrontOfOther += (thisFromOther >= 0.0f) ? 1 : 0; 
		}

		//We are not intersecting if there are exactly 4 in front of either plane. Early out bro
		if ((inFrontOfThis == 4) || (inFrontOfOther == 4)) {
			return false;
		}
	}

	//We have reached this point. There's an intersection for sure
	return true; 
}

OBB2::~OBB2()
{

}

