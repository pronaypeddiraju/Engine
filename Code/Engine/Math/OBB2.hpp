//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Math/AABB2.hpp"

//------------------------------------------------------------------------------------------------------------------------------
class Plane2D;
class Segment2D;

//------------------------------------------------------------------------------------------------------------------------------
class OBB2
{
public:
	OBB2();
	~OBB2();
	explicit OBB2( Vec2 center, Vec2 size = Vec2::ZERO, float rotationDegrees = 0.0f ); 
	explicit OBB2( AABB2 const &aabb ); 

	//Modification Utilities
	void Translate( Vec2 offset );
	void MoveTo( Vec2 position );

	void RotateBy( float degrees );
	void SetRotation( float degrees );

	void SetSize( Vec2 newSize );

	//Accessors for OBB Properties
	inline const Vec2& GetRight() const				{ return m_right; }
	inline const Vec2& GetUp() const				{ return m_up; }
	inline const Vec2& GetCenter() const			{ return m_center; }
	inline const Vec2& GetHalfExtents() const				{ return m_halfExtents; }

	//Accessors for the corners themselves
	inline Vec2 GetBottomLeft() const		{ return m_center - m_halfExtents.x * GetRight() - m_halfExtents.y * GetUp(); }
	inline Vec2 GetBottomRight() const		{ return m_center + m_halfExtents.x * GetRight() - m_halfExtents.y * GetUp(); }
	inline Vec2 GetTopLeft() const			{ return m_center - m_halfExtents.x * GetRight() + m_halfExtents.y * GetUp(); }
	inline Vec2 GetTopRight() const			{ return m_center + m_halfExtents.x * GetRight() + m_halfExtents.y * GetUp(); }

	// Collision Utility
	Vec2 ToLocalPoint( Vec2 worldPoint ) const; 
	Vec2 ToWorldPoint( Vec2 localPoint ) const; 

	void GetPlanes(Plane2D* out) const;
	void GetCorners(Vec2* out) const;
	void GetSides(Segment2D* out) const;

	bool Contains( Vec2 worldPoint ) const;
	Vec2 GetClosestPoint( Vec2 worldPoint ) const;

	bool Intersects( OBB2 const &other ) const; 

public:
	Vec2 m_right         = Vec2( 1.0f, 0.0f ); 
	Vec2 m_up            = Vec2( 0.0f, 1.0f );
	Vec2 m_center        = Vec2( 0.0f, 0.0f ); 
	Vec2 m_halfExtents   = Vec2( 0.0f, 0.0f ); 
};