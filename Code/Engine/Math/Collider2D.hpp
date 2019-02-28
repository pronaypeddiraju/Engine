#pragma once
//------------------------------------------------------------------------------------------------------------------------------
//Engine Systems
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Disc2D.hpp"

class Rigidbody2D;
struct Collision2D;

enum eColliderType2D
{
	COLLIDER_UNKOWN = -1,

	COLLIDER_AABB2, 
	COLLIDER_DISC,

	// more to come;
	// COLLIDER_CAPSULE, 
	// COLLIDER_BOX,

	NUM_COLLIDER_TYPES
};

class Collider2D
{
public:
	bool						IsTouching(Collision2D* collision, Collider2D* otherCollider);
	eColliderType2D				GetType();
	void						SetCollision(bool inCollision);

public:
	Rigidbody2D*				m_rigidbody = nullptr;
	eColliderType2D				m_colliderType = COLLIDER_UNKOWN;

	bool						m_inCollision = false;
};

class AABB2Collider: public Collider2D
{
public:
	explicit AABB2Collider(const Vec2& minBounds, const Vec2& maxBounds);
	~AABB2Collider();

	AABB2						GetLocalShape() const;		//Shape relative to rigidbody
	AABB2						GetWorldShape() const;		//Shape in world

	Vec2						GetBoxCenter() const;
public:
	AABB2						m_localShape;
};

class Disc2DCollider: public Collider2D
{
public:
	explicit Disc2DCollider(const Vec2& centre, float radius);
	~Disc2DCollider();

	Disc2D						GetLocalShape() const;
	Disc2D						GetWorldShape() const;

public:
	Disc2D						m_localShape;
};