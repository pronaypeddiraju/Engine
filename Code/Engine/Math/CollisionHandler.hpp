#pragma once
//------------------------------------------------------------------------------------------------------------------------------
//Engine Systems
#include "Engine/Commons/EngineCommon.hpp"
#include "Engine/Commons/ErrorWarningAssert.hpp"
#include "Engine/Math/Manifold.hpp"

#include <functional>

typedef unsigned int uint;
class Collider2D;
class AABB2Collider;
class Disc2DCollider;
class BoxCollider2D;
class CapsuleCollider2D;
class OBB2;
class Capsule2D;
struct AABB2;

//------------------------------------------------------------------------------------------------------------------------------
struct Collision2D
{
	Collider2D *m_Obj; 
	Collider2D *m_otherObj; 
	Manifold2D m_manifold; // may be referred to as a "contact"

	void InvertCollision();
};

typedef std::function<bool(Collision2D* out, Collider2D* a, Collider2D* b)> CollisionCheck2DCallback ;
// 2D arrays are [Y][X] remember
extern CollisionCheck2DCallback COLLISION_LOOKUP_TABLE[][COLLIDER2D_COUNT];

//------------------------------------------------------------------------------------------------------------------------------
bool				CheckAABB2ByAABB2(Collision2D* out, Collider2D* a, Collider2D* b );
bool				CheckAABB2ByDisc(Collision2D* out, Collider2D* a, Collider2D* b );
bool				CheckDiscByDisc(Collision2D* out, Collider2D* a, Collider2D* b );
bool				CheckDiscByAABB2(Collision2D* out, Collider2D* a, Collider2D* b );
bool				CheckOBB2ByOBB2(Collision2D* out, Collider2D* a, Collider2D* b);
bool				CheckCapsuleByCapsule(Collision2D* out, Collider2D* a, Collider2D* b);
bool				CheckCapsuleByOBB2(Collision2D* out, Collider2D* a, Collider2D* b);
bool				CheckOBB2ByCapsule(Collision2D* out, Collider2D* a, Collider2D* b);
bool				GetCollisionInfo( Collision2D *out, Collider2D * a, Collider2D *b );

//------------------------------------------------------------------------------------------------------------------------------
//Manifold Generation
//------------------------------------------------------------------------------------------------------------------------------
bool				GetManifold( Manifold2D *out, AABB2Collider const &obj0, AABB2Collider const &obj1 ); 
bool				GetManifold( Manifold2D *out, AABB2Collider const &obj0, Disc2DCollider const &obj1 ); 
bool				GetManifold( Manifold2D *out, Disc2DCollider const &obj0, Disc2DCollider const &obj1 );
bool				GetManifold( Manifold2D *out, Disc2DCollider const &disc, AABB2Collider const &box );

//------------------------------------------------------------------------------------------------------------------------------
//OBB to OBB and Pillbox to Pillbox collisions
//------------------------------------------------------------------------------------------------------------------------------
bool				GetManifold( Manifold2D *out, OBB2 const &boxA, OBB2 const &boxB );
bool				GetManifold( Manifold2D *out, BoxCollider2D const &a, BoxCollider2D const &b );
bool				GetManifold( Manifold2D *out, BoxCollider2D const &a, float aRadius, BoxCollider2D const &b, float bRadius );
bool				GetManifold( Manifold2D *out, OBB2 const &a, float aRadius, OBB2 const &b, float bRadius );

//------------------------------------------------------------------------------------------------------------------------------
//Capsules and OBBs Collision Manifolds
//------------------------------------------------------------------------------------------------------------------------------
bool				GetManifold( Manifold2D *out, CapsuleCollider2D const &a, CapsuleCollider2D const &b );
bool				GetManifold( Manifold2D *out, BoxCollider2D const &a, CapsuleCollider2D const &b );
bool				GetManifold( Manifold2D *out, CapsuleCollider2D const &a, BoxCollider2D const &b );

bool				IsDiscInBox( Manifold2D* out, const Vec2 &discCentre, const AABB2& boxShape, float radius );

//------------------------------------------------------------------------------------------------------------------------------
//Manifold Helpers
//------------------------------------------------------------------------------------------------------------------------------
void				GenerateManifoldBoxToBox( Manifold2D* manifold, Vec2 const &min, Vec2 const &max );