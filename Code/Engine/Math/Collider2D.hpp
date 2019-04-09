#pragma once
//------------------------------------------------------------------------------------------------------------------------------
//Engine Systems
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Capsule2D.hpp"
#include "Engine/Math/Disc2D.hpp"
#include "Engine/Math/OBB2.hpp"

class Rigidbody2D;
struct Collision2D;

//------------------------------------------------------------------------------------------------------------------------------
enum eColliderType2D
{
	COLLIDER_UNKOWN = -1,

	COLLIDER_AABB2, 
	COLLIDER_DISC,

	COLLIDER_CAPSULE, 
	COLLIDER_BOX,

	NUM_COLLIDER_TYPES
};

//------------------------------------------------------------------------------------------------------------------------------
class Collider2D
{
public:

	virtual void				SetMomentForObject() = 0;
	virtual bool				Contains(Vec2 worldPoint) = 0;

	bool						IsTouching(Collision2D* collision, Collider2D* otherCollider);
	eColliderType2D				GetType();
	void						SetCollision(bool inCollision);

public:
	Rigidbody2D*				m_rigidbody = nullptr;
	eColliderType2D				m_colliderType = COLLIDER_UNKOWN;

	bool						m_inCollision = false;
};

//------------------------------------------------------------------------------------------------------------------------------
class AABB2Collider: public Collider2D
{
public:
	explicit AABB2Collider(const Vec2& minBounds, const Vec2& maxBounds);
	~AABB2Collider();

	virtual void				SetMomentForObject();
	virtual bool				Contains(Vec2 worldPoint);

	AABB2						GetLocalShape() const;		//Shape relative to rigidbody
	AABB2						GetWorldShape() const;		//Shape in world

	Vec2						GetBoxCenter() const;
public:
	AABB2						m_localShape;
};

//------------------------------------------------------------------------------------------------------------------------------
class Disc2DCollider: public Collider2D
{
public:
	explicit Disc2DCollider(const Vec2& centre, float radius);
	~Disc2DCollider();

	virtual void				SetMomentForObject();
	virtual bool				Contains(Vec2 worldPoint);

	Disc2D						GetLocalShape() const;
	Disc2D						GetWorldShape() const;

public:
	Disc2D						m_localShape;
};

//------------------------------------------------------------------------------------------------------------------------------
class BoxCollider2D: public Collider2D
{
public:
	explicit BoxCollider2D( Vec2 center, Vec2 size = Vec2::ZERO, float rotationDegrees = 0.0f );
	~BoxCollider2D();

	virtual void				SetMomentForObject();
	virtual bool				Contains(Vec2 worldPoint);

	OBB2						GetLocalShape() const;
	OBB2						GetWorldShape() const;

public:
	OBB2						m_localShape;
};

//------------------------------------------------------------------------------------------------------------------------------
class CapsuleCollider2D: public Collider2D
{
public:
	explicit CapsuleCollider2D ( Vec2 start, Vec2 end, float radius );
	~CapsuleCollider2D();

	virtual void				SetMomentForObject();
	virtual bool				Contains(Vec2 worldPoint);

	OBB2						GetLocalShape() const;
	OBB2						GetWorldShape() const;

	const Capsule2D&			GetReferenceShape() const;

	float						GetCapsuleRadius() const;

public:
	Capsule2D					m_referenceCapsule;

	OBB2						m_localShape;
	float						m_radius;
};