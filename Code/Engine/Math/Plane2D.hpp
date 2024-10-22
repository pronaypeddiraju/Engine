//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Commons/EngineCommon.hpp"

//------------------------------------------------------------------------------------------------------------------------------
class Plane2D
{
public:
	Plane2D();
	Plane2D( Vec2 normal, float distanceToOrigin );
	Plane2D( Vec2 normal, Vec2 pointOnPlane);

	// positive -> in front of the plane
	// negative -> behind the plane; 
	float				GetDistance( const Vec2& point ) const;
	Vec2				GetNormal() const { return m_normal; }
	float				GetSignedDistance() const { return m_signedDistance; }
	inline bool			IsPointInFrontOfPlane( const Vec2& point ) const { return (GetDistance(point) > 0.0f); }
	inline bool			IsPointBehindPlane( const Vec2& point ) const { return (GetDistance(point) < 0.0f); }

	bool				operator==(const Plane2D& compare) const;

public:
	Vec2				m_normal; 
	float				m_signedDistance; // distance along normal to get to the origin; 
		
public:
	// named constructors;
	static Plane2D		AtPosition( const Vec2& pointOnPlane, const Vec2& normal ); 
	static Plane2D		FromPoints( const Vec2& p0, const Vec2& p1 ); 
};