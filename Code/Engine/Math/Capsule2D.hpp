//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Commons/EngineCommon.hpp"

struct AABB2;

//------------------------------------------------------------------------------------------------------------------------------
class Capsule2D
{
public:
	Capsule2D(); 
	Capsule2D( Vec2 pos );                  // Equivalent to a point; 
	Capsule2D( Vec2 center, float radius ); // Equivalent to a disc; 
	Capsule2D( Vec2 p0, Vec2 p1, float radius ); 
	~Capsule2D();

	// Modification Utility
	void		SetPosition( Vec2 pos );                         // special care here;   Use the center of the line as position, but maintain shape
	void		SetPositions( Vec2 p0, Vec2 p1 ); 
	void		Translate( Vec2 offset ); 

	void		RotateBy( float degrees );                       // A04
				
	// Helpers for describing it; 
	inline const Vec2&		GetStart() const { return m_start; };
	inline const Vec2&		GetEnd() const {return m_end; }; 

	// Collision Utility
	bool		Contains( Vec2 worldPoint ); 
	Vec2		GetClosestPoint( Vec2 worldPoint ); 

	// Useful if you want to use AABB2 as a "broad phase" & "mid phase" check
	// like checking if something fell outside the world
	AABB2		GetBoundingAABB() const; 

public:
	Vec2 m_start         = Vec2::ZERO;
	Vec2 m_end           = Vec2::ZERO; 
	float m_radius       = 0.0f; 
	// Note: defaults basically make a "point" at 0; 
};

