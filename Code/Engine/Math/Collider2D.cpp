//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Math/Collider2D.hpp"
#include "Engine/Math/CollisionHandler.hpp"
#include "Engine/Math/Rigidbody2D.hpp"
#include "Engine/Math/Disc2D.hpp"
#include "Engine/Math/MathUtils.hpp"

//------------------------------------------------------------------------------------------------------------------------------
bool Collider2D::IsTouching(Collision2D* collision, Collider2D* otherCollider )
{
	return GetCollisionInfo( collision, this, otherCollider); 
}

eColliderType2D Collider2D::GetType()
{
	return m_colliderType;
}

void Collider2D::SetCollision( bool inCollision )
{
	m_inCollision = inCollision;
}

AABB2Collider::AABB2Collider( const Vec2& minBounds, const Vec2& maxBounds )
{
	m_localShape = AABB2(minBounds, maxBounds);
}

AABB2Collider::~AABB2Collider()
{

}

void AABB2Collider::SetMomentForObject()
{
	GUARANTEE_OR_DIE(true, "There is no implementation for handling the Moment of Inertia for a AABB2");
}

AABB2 AABB2Collider::GetLocalShape() const
{
	return m_localShape;
}

AABB2 AABB2Collider::GetWorldShape() const
{
	AABB2 box = GetLocalShape();
	box.TranslateByVector(m_rigidbody->GetPosition());
	return box;
}

Vec2 AABB2Collider::GetBoxCenter() const
{
	return m_localShape.GetBoxCenter();
}

Disc2DCollider::Disc2DCollider( const Vec2& centre, float radius )
{
	m_localShape = Disc2D(centre, radius);
}

Disc2DCollider::~Disc2DCollider()
{

}

void Disc2DCollider::SetMomentForObject()
{
	GUARANTEE_OR_DIE(true, "There is no implementation for handling the Moment of Inertia for a Disc");
}

Disc2D Disc2DCollider::GetLocalShape() const
{
	return m_localShape;
}

Disc2D Disc2DCollider::GetWorldShape() const
{
	Disc2D disc = GetLocalShape();
	disc.TranslateByVector(m_rigidbody->GetPosition());
	return disc;
}

BoxCollider2D::BoxCollider2D( Vec2 center, Vec2 size /*= Vec2::ZERO*/, float rotationDegrees /*= 0.0f */ )
{
	m_localShape = OBB2(center, size, rotationDegrees);
}

BoxCollider2D::~BoxCollider2D()
{

}

void BoxCollider2D::SetMomentForObject()
{
	float width = (m_localShape.m_halfExtents.x * 2.f); 
	float height = (m_localShape.m_halfExtents.y * 2.f); 

	//0.08333333333 = 1/12 which is what we will need for the Moment of inertia of a box. I avoid the / operation like this 
	m_rigidbody->m_momentOfInertia = m_rigidbody->m_mass * (1.f/12.f * (width * width + height * height));
}

OBB2 BoxCollider2D::GetLocalShape() const
{
	return m_localShape;
}

OBB2 BoxCollider2D::GetWorldShape() const
{
	OBB2 box = GetLocalShape();
	box.Translate(m_rigidbody->GetPosition());
	return box;
}

CapsuleCollider2D::CapsuleCollider2D( Vec2 start, Vec2 end, float radius )
{
	m_referenceCapsule = Capsule2D(start, end, radius);
	
	Vec2 disp = start - end;
	float dist = disp.GetLength();

	float rotationDegrees = disp.GetAngleDegrees();

	m_localShape = OBB2( Vec2::ZERO, Vec2(0.f , dist), rotationDegrees + 90.f);
	m_radius = radius;
}

CapsuleCollider2D::~CapsuleCollider2D()
{

}

void CapsuleCollider2D::SetMomentForObject()
{

	//The mass of the disc and box component should be calculated as ratios of original mass
	//Split the capsule into 2 half-discs and a box
	float discArea = PI * m_radius * m_radius;
	float boxArea = (2.f * m_radius) * (m_localShape.m_halfExtents.y * 2.f);

	float netArea = boxArea + discArea;
	float ratioDisc = discArea / netArea;
	float ratioBox = boxArea / netArea;

	//Box component
	float width = m_radius; 
	float height = (m_localShape.m_halfExtents.y * 2.f); 

	//0.08333333333 = 1/12 which is what we will need for the Moment of inertia of a box. I avoid the / operation like this 
	m_rigidbody->m_momentOfInertia = (0.08333333333f * (width * width + height * height)) * ratioBox * m_rigidbody->m_mass;

	float offset = GetDistance2D(m_rigidbody->GetPosition(), m_localShape.GetBottomLeft());

	//Disc component
	m_rigidbody->m_momentOfInertia += 0.5f * m_rigidbody->m_mass * ratioDisc * m_radius * m_radius;
	//Point component
	m_rigidbody->m_momentOfInertia += offset * offset * m_rigidbody->m_mass * ratioDisc;
}

OBB2 CapsuleCollider2D::GetLocalShape() const
{
	return m_localShape;
}

OBB2 CapsuleCollider2D::GetWorldShape() const
{
	OBB2 box = GetLocalShape();
	box.Translate(m_rigidbody->GetPosition());
	return box;
}

const Capsule2D& CapsuleCollider2D::GetReferenceShape() const
{
	return m_referenceCapsule;
}

float CapsuleCollider2D::GetCapsuleRadius() const
{
	return m_radius;
}
