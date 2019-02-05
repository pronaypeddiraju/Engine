//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Math/Collider2D.hpp"
#include "Engine/Math/CollisionHandler.hpp"
#include "Engine/Math/Rigidbody2D.hpp"
#include "Engine/Math/Disc2D.hpp"

bool Collider2D::IsTouching( Collider2D* otherCollider )
{
	Collision2D collision; 
	return GetCollisionInfo( &collision, this, otherCollider); 
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

AABB2 AABB2Collider::GetLocalShape()
{
	return m_localShape;
}

AABB2 AABB2Collider::GetWorldShape()
{
	AABB2 box = GetLocalShape();
	box.TranslateByVector(m_rigidbody->GetPosition());
	return box;
}

Disc2DCollider::Disc2DCollider( const Vec2& centre, float radius )
{
	m_localShape = Disc2D(centre, radius);
}

Disc2DCollider::~Disc2DCollider()
{

}

Disc2D Disc2DCollider::GetLocalShape()
{
	return m_localShape;
}

Disc2D Disc2DCollider::GetWorldShape()
{
	Disc2D disc = GetLocalShape();
	disc.TranslateByVector(m_rigidbody->GetPosition());
	return disc;
}

