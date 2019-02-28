//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Math/CollisionHandler.hpp"
#include "Engine/Math/Collider2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Manifold.hpp"

//------------------------------------------------------------------------------------------------------------------------------
CollisionCheck2DCallback COLLISION_LOOKUP_TABLE[COLLIDER2D_COUNT][COLLIDER2D_COUNT] = {
	/*******| aabb2 | disc  | point | capsl | line  | obb2  */
	/*aabb2*/ { CheckAABB2ByAABB2, CheckAABB2ByDisc, nullptr,           nullptr, nullptr, nullptr },
	/*disc */ { CheckDiscByAABB2,  CheckDiscByDisc,  nullptr,           nullptr, nullptr, nullptr },
	/*point*/ { nullptr,           nullptr,          nullptr,           nullptr, nullptr, nullptr },
	/*capsl*/ { nullptr,           nullptr,          nullptr,           nullptr, nullptr, nullptr },
	/*line */ { nullptr,           nullptr,          nullptr,           nullptr, nullptr, nullptr },
	/*obb2 */ { nullptr,           nullptr,          nullptr,           nullptr, nullptr, nullptr }
}; 

//------------------------------------------------------------------------
// Doing this by lookup - as it is a good intro to callbacks
// but could also do this by double dispatch:  a->collides_with( b )
bool GetCollisionInfo( Collision2D* out, Collider2D* a, Collider2D* b )
{
	uint aType = a->GetType(); 
	uint bType = b->GetType(); 
	
	if(aType >= COLLIDER2D_COUNT && bType >= COLLIDER2D_COUNT)
	{
		ERROR_AND_DIE("The Collider type was not part of the COLLISION_LOOKUP_TABLE");
	}

	CollisionCheck2DCallback callBack = COLLISION_LOOKUP_TABLE[aType][bType]; 
	
	if (callBack == nullptr) 
	{
		return false; // no known collision; 
	}
	else 
	{
		return callBack( out, a, b ); 
	}
}

bool GetManifold( Manifold2D *out, AABB2Collider const &boxA, AABB2Collider const &boxB )
{
	//Get the intersecting box
	Vec2 min = boxA.GetWorldShape().m_maxBounds.Min(boxB.GetWorldShape().m_maxBounds);
	Vec2 max = boxA.GetWorldShape().m_minBounds.Max(boxB.GetWorldShape().m_minBounds);

	if(max < min)
	{
		GenerateManifoldBoxToBox(out, min, max);
		if(out->m_normal.y == 0.f)
		{
			//pushing out on x
			if(boxA.GetWorldShape().m_center.x < boxB.GetWorldShape().m_center.x)
			{
				out->m_normal *= -1;
			}
		}
		return true;
	}
	else 
	{
		return false;
	}
}

bool GetManifold( Manifold2D *out, AABB2Collider const &box, Disc2DCollider const &disc )
{
	Vec2 discCentre = disc.GetWorldShape().GetCentre();

	AABB2 boxShape = box.GetWorldShape();
	Vec2 closestPoint = GetClosestPointOnAABB2( discCentre, boxShape );

	float distanceSquared = GetDistanceSquared2D(discCentre, closestPoint);
	float radius = disc.GetWorldShape().GetRadius();

	if(distanceSquared < radius * radius)
	{
		//out here
		float distance = GetDistance2D(discCentre, closestPoint);
		Vec2 normal = closestPoint - discCentre;
		normal.Normalize();

		out->m_normal = normal;
		out->m_penetration = radius - distance;
		return true;
	}
	else
	{
		return false;
	}
}

bool GetManifold( Manifold2D *out, Disc2DCollider const &disc, AABB2Collider const &box)
{
	Vec2 discCentre = disc.GetWorldShape().GetCentre();
	AABB2 boxShape = box.GetWorldShape();

	Vec2 closestPoint = GetClosestPointOnAABB2( discCentre, boxShape );

	float distanceSquared = GetDistanceSquared2D(discCentre, closestPoint);
	float radius = disc.GetWorldShape().GetRadius();

	if(closestPoint == discCentre)
	{
		return IsDiscInBox(out, discCentre, boxShape, radius);
	}

	if(distanceSquared < radius * radius)
	{
		//out here
		float distance = GetDistance2D(discCentre, closestPoint);
		Vec2 normal = discCentre - closestPoint;
		normal.Normalize();

		out->m_normal = normal;
		out->m_penetration = radius - distance;
		return true;
	}
	else
	{
		return false;
	}
}

bool IsDiscInBox(Manifold2D* out, const Vec2 &discCentre, const AABB2& boxShape, float radius)
{
	//Disc is either inside box or not colliding
	Vec2 horNormal;
	Vec2 vertNormal;
	Vec2 normal;
	float distance = 0.f;

	if(discCentre < boxShape.m_maxBounds && discCentre > boxShape.m_minBounds)
	{
		//Disc is inside the box
		float horizontalDist;
		float rightToDisc = boxShape.m_maxBounds.x - discCentre.x;
		float leftToDisc = discCentre.x - boxShape.m_minBounds.x;
		if( rightToDisc > leftToDisc )
		{
			horizontalDist = leftToDisc;
			horNormal = Vec2(-1.f, 0.f);
		}
		else
		{
			horizontalDist = rightToDisc;
			horNormal = Vec2(1.f, 0.f);
		}

		float vertDistance;
		float topToDisc = boxShape.m_maxBounds.y - discCentre.y;
		float botToDisc = discCentre.y - boxShape.m_minBounds.y;
		if( topToDisc > botToDisc )
		{
			vertDistance = botToDisc;
			vertNormal = Vec2(0.f, -1.f);
		}
		else
		{
			vertDistance = topToDisc;
			vertNormal = Vec2(0.f, 1.f);
		}

		//Figure which axis to push on
		if(horizontalDist > vertDistance)
		{
			normal = vertNormal;
			distance = vertDistance;
		}
		else
		{
			normal = horNormal;
			distance = horizontalDist;
		}

		out->m_normal = normal;
		out->m_penetration = distance + radius;
		return true;
	}
	else
	{
		//No collision
		return false;
	}
}

bool GetManifold( Manifold2D *out, Disc2DCollider const &discA, Disc2DCollider const &discB )
{
	float discARad = discA.GetWorldShape().GetRadius();
	float discBRad = discB.GetWorldShape().GetRadius();

	Vec2 discACenter = discA.GetWorldShape().GetCentre();
	Vec2 discBCenter = discB.GetWorldShape().GetCentre();
	float distanceSquared = GetDistanceSquared2D(discACenter, discBCenter);
	float radSumSquared = (discARad + discBRad) * (discARad + discBRad);

	if(distanceSquared < radSumSquared)
	{
		float distance = GetDistance2D(discACenter, discBCenter);
		Vec2 normal = discACenter - discBCenter;
		normal.Normalize();

		out->m_normal = normal;
		out->m_penetration = discARad + discBRad - distance;
		return true;
	}
	else
	{
		return false;
	}
}

void GenerateManifoldBoxToBox(Manifold2D* manifold, Vec2 const &min, Vec2 const &max)
{
	//Remember since we collided, the max is actually manifold min and min is actually manifold max
	float boxWidth = min.x - max.x;
	float boxHeight = min.y - max.y;
	float minValue = GetLowerValue(boxWidth, boxHeight);

	Vec2 normal;

	//Where is boxB wrt to boxA
	if(minValue == boxWidth)
	{
		//normal is along X
		normal = Vec2(min.x, 0.f) - Vec2(max.x, 0.f);
	}
	else
	{
		//Normal is along Y
		normal = Vec2(0.f, min.y) - Vec2(0.f, max.y);
	}

	normal.Normalize();

	manifold->m_normal = normal;
	manifold->m_penetration = minValue;
}

bool CheckAABB2ByAABB2(Collision2D* out, Collider2D* a, Collider2D* b)
{
	//Check collision between 2 boxes
	AABB2Collider* boxA = reinterpret_cast<AABB2Collider*>(a);
	AABB2Collider* boxB = reinterpret_cast<AABB2Collider*>(b);

	Manifold2D manifold;
	bool result = GetManifold(&manifold, *boxA, *boxB);

	if(result)
	{
		//Manifold is valid
		out->m_Obj = a;
		out->m_otherObj = b;
		out->m_manifold = manifold;
	}
	else
	{
		//Invalid manifold as no collision
		out->m_Obj = nullptr;
		out->m_otherObj = nullptr;
	}
	
	return result;
}

bool CheckAABB2ByDisc(Collision2D* out, Collider2D* a, Collider2D* b)
{
	//box vs disc
	AABB2Collider* boxA = reinterpret_cast<AABB2Collider*>(a);
	Disc2DCollider* discB = reinterpret_cast<Disc2DCollider*>(b);

	Manifold2D manifold;
	bool result = GetManifold(&manifold, *boxA, *discB);

	if(result)
	{
		out->m_manifold = manifold;
		out->m_Obj = a;
		out->m_otherObj = b;
	}
	else
	{
		out->m_Obj = nullptr;
		out->m_otherObj = nullptr;
	}

	return result;
}

bool CheckDiscByDisc(Collision2D* out, Collider2D* a, Collider2D* b)
{
	//disc vs disc
	Disc2DCollider* discA = reinterpret_cast<Disc2DCollider*>(a);
	Disc2DCollider* discB = reinterpret_cast<Disc2DCollider*>(b);

	Manifold2D manifold;
	bool result = GetManifold(&manifold, *discA, *discB);

	if(result)
	{
		// out here
		out->m_Obj = a;
		out->m_otherObj = b;
		out->m_manifold = manifold;
		return true;
	}
	else
	{
		out->m_otherObj = nullptr;
		out->m_Obj = nullptr;
		return false;
	}
}

bool CheckDiscByAABB2(Collision2D* out, Collider2D* a, Collider2D* b)
{
	//disc vs box
	Disc2DCollider* discA = reinterpret_cast<Disc2DCollider*>(a);
	AABB2Collider* boxB = reinterpret_cast<AABB2Collider*>(b);

	Manifold2D manifold;
	bool result = GetManifold(&manifold, *discA, *boxB);

	if(result)
	{
		//out here
		out->m_Obj = a;
		out->m_otherObj = b;
		out->m_manifold = manifold;
		return true;
	}
	else
	{
		out->m_Obj = nullptr;
		out->m_otherObj = nullptr;
		return false;
	}
}