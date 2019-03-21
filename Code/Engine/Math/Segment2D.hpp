//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Commons/EngineCommon.hpp"

//------------------------------------------------------------------------------------------------------------------------------
class Segment2D 
{
public:
	Segment2D();
	Segment2D( Vec2 start, Vec2 end );
	~Segment2D();

	inline Vec2 const& GetStart() const        { return m_start; }
	inline Vec2 const& GetEnd() const          { return m_end; }
	
	Vec2 GetCenter() const; 
	Vec2 GetDirection() const; 

	// This is what we actually need; 
	// returns closest point on line segment (voronoi regions.  Either on the segment, or one of the end points)
	Vec2 GetClosestPoint( Vec2 pos ) const; 

public:
	Vec2 m_start;
	Vec2 m_end; 
}; 


