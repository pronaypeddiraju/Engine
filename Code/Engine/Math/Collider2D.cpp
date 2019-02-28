//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Math/Collider2D.hpp"
#include "Engine/Math/CollisionHandler.hpp"
#include "Engine/Math/Rigidbody2D.hpp"
#include "Engine/Math/Disc2D.hpp"

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

