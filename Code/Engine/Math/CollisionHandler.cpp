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
	
	Vec2 min = boxA->m_localShape.m_minBounds.Min(boxB->m_localShape.m_minBounds);
	Vec2 max = boxA->m_localShape.m_maxBounds.Max(boxB->m_localShape.m_maxBounds);

	if(min < max)
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

	Vec2 discCentre = discB->m_localShape.GetCentre();
	
	Vec2 closestPoint = GetClosestPointOnAABB2( discCentre, boxA->m_localShape );

	float distanceSquared = GetDistanceSquared2D(discCentre, closestPoint);
	float radius = discB->m_localShape.GetRadius();

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

	float discARad = discA->m_localShape.GetRadius();
	float discBRad = discB->m_localShape.GetRadius();

	float distanceSquared = GetDistanceSquared2D(discA->m_localShape.GetCentre(), discB->m_localShape.GetCentre());
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

	Vec2 discCentre = discA->m_localShape.GetCentre();

	Vec2 closestPoint = GetClosestPointOnAABB2( discCentre, boxB->m_localShape );

	float distanceSquared = GetDistanceSquared2D(discCentre, closestPoint);
	float radius = discA->m_localShape.GetRadius();

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