//------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/AABB2.hpp"

//------------------------------------------------------------------------------------------------------------------------------
struct AABB3
{
public:
	AABB3();
	~AABB3();
	explicit AABB3(const Vec3& frontMinBounds,const Vec3& frontMaxBounds, float boxDepth);

	//Methods to get faces
	const AABB2&			GetFrontFace() const;
	const AABB2&			GetBackFace() const;
	const AABB2&			GetTopFace() const;
	const AABB2&			GetBottomFace() const;
	const AABB2&			GetLeftFace() const;
	const AABB2&			GetRightFace() const;

	void					ConstructAllFaces();

public:
	//Front face
	Vec3					m_frontTopLeft		= Vec3::ZERO;
	Vec3					m_frontTopRight		= Vec3::ZERO;
	Vec3					m_frontBottomLeft	= Vec3::ZERO;
	Vec3					m_frontBottomRight	= Vec3::ZERO;
												
	//Back face									
	Vec3					m_backTopLeft		= Vec3::ZERO;
	Vec3					m_backTopRight		= Vec3::ZERO;
	Vec3					m_backBottomLeft	= Vec3::ZERO;
	Vec3					m_backBottomRight	= Vec3::ZERO;

	//faces
	AABB2					m_frontFace;
	AABB2					m_backFace;
	AABB2					m_topFace;
	AABB2					m_bottomFace;
	AABB2					m_leftFace;
	AABB2					m_rightFace;

};