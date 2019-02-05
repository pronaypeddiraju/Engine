//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Math/CollisionHandler.hpp"
#include "Engine/Math/Collider2D.hpp"
#include "Engine/Math/MathUtils.hpp"

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
		//When Collision2D is made, we will use this part of the Code

		//out->me = a; 
		//out->them = b; 

		return callBack( out, a, b ); 
	}
}

bool CheckAABB2ByAABB2(Collision2D* out, Collider2D* a, Collider2D* b)
{
	UNUSED(out);
	//Check collision between 2 boxes
	AABB2Collider* boxA = reinterpret_cast<AABB2Collider*>(a);
	AABB2Collider* boxB = reinterpret_cast<AABB2Collider*>(b);
	
	Vec2 min = boxA->GetWorldShape().m_maxBounds.Min(boxB->GetWorldShape().m_maxBounds);
	Vec2 max = boxA->GetWorldShape().m_minBounds.Max(boxB->GetWorldShape().m_minBounds);

	if(max < min)
	{
		//*out = AABB2(min, max);
		return true;
	}
	else 
	{
		return false;
	}
}

bool CheckAABB2ByDisc(Collision2D* out, Collider2D* a, Collider2D* b)
{
	UNUSED(out);
	//box vs disc
	AABB2Collider* boxA = reinterpret_cast<AABB2Collider*>(a);
	Disc2DCollider* discB = reinterpret_cast<Disc2DCollider*>(b);

	Vec2 discCentre = discB->GetWorldShape().GetCentre();
	
	Vec2 closestPoint = GetClosestPointOnAABB2( discCentre, boxA->GetWorldShape() );

	float distanceSquared = GetDistanceSquared2D(discCentre, closestPoint);
	float radius = discB->GetWorldShape().GetRadius();

	if(distanceSquared < radius * radius)
	{
		//out here
		return true;
	}
	else
	{
		return false;
	}
}

bool CheckDiscByDisc(Collision2D* out, Collider2D* a, Collider2D* b)
{
	UNUSED(out);
	//disc vs disc
	Disc2DCollider* discA = reinterpret_cast<Disc2DCollider*>(a);
	Disc2DCollider* discB = reinterpret_cast<Disc2DCollider*>(b);

	float discARad = discA->GetWorldShape().GetRadius();
	float discBRad = discB->GetWorldShape().GetRadius();

	float distanceSquared = GetDistanceSquared2D(discA->GetWorldShape().GetCentre(), discB->GetWorldShape().GetCentre());
	float radSumSquared = (discARad + discBRad) * (discARad + discBRad);

	if(distanceSquared < radSumSquared)
	{
		// out here
		return true;
	}
	else
	{
		return false;
	}
}

bool CheckDiscByAABB2(Collision2D* out, Collider2D* a, Collider2D* b)
{
	UNUSED(out);
	//disc vs box
	Disc2DCollider* discA = reinterpret_cast<Disc2DCollider*>(a);
	AABB2Collider* boxB = reinterpret_cast<AABB2Collider*>(b);

	Vec2 discCentre = discA->GetWorldShape().GetCentre();

	Vec2 closestPoint = GetClosestPointOnAABB2( discCentre, boxB->GetWorldShape() );

	float distanceSquared = GetDistanceSquared2D(discCentre, closestPoint);
	float radius = discA->GetWorldShape().GetRadius();

	if(distanceSquared < radius * radius)
	{
		//out here
		return true;
	}
	else
	{
		return false;
	}
}