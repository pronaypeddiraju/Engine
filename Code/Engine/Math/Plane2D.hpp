//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Commons/EngineCommon.hpp"

//------------------------------------------------------------------------------------------------------------------------------
class Plane2D
{
public:
	Plane2D();
	Plane2D( Vec2 normal, float distanceToOrigin );

	// positive -> in front of the plane
	// negative -> behind the plane; 
	float				GetDistance( Vec2 point ) const;
	inline bool			IsInFront( Vec2 point ) const { return (GetDistance(point) > 0.0f); }
	inline bool			IsBehind( Vec2 point ) const { return (GetDistance(point) < 0.0f); }

	// Not needed for this class, but useful
	// Vec2 GetProjectedPoint( Vec2 point ) const; 

public:
	Vec2				m_normal; 
	float				m_distance; // distance along normal to get to the origin; 
		
public:
	// named constructors;
	static Plane2D		AtPosition( Vec2 pointOnPlane, Vec2 normal ); 
	static Plane2D		FromPoints( Vec2 p0, Vec2 p1 ); 
};