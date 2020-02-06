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
	float				GetDistance( Vec2 point ) const;
	inline bool			IsPointInFrontOfPlane( Vec2 point ) const { return (GetDistance(point) > 0.0f); }
	inline bool			IsPointBehindPlane( Vec2 point ) const { return (GetDistance(point) < 0.0f); }

	bool				operator==(const Plane2D& compare) const;

public:
	Vec2				m_normal; 
	float				m_signedDistance; // distance along normal to get to the origin; 
		
public:
	// named constructors;
	static Plane2D		AtPosition( Vec2 pointOnPlane, Vec2 normal ); 
	static Plane2D		FromPoints( Vec2 p0, Vec2 p1 ); 
};